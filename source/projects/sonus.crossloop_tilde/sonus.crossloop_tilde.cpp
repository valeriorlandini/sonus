/// @file       sonus.crossloop_tilde.cpp
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <cmath>
#include <sys/stat.h>
#include <utility>
#include "interp.h"

using namespace c74::min;
using namespace soutel;

class crossloop_tilde : public object<crossloop_tilde>, public vector_operator<>
{
public:
    MIN_DESCRIPTION	{ "Loop player with crossfade." };
    MIN_TAGS		{ "audio, sampling" };
    MIN_AUTHOR		{ "Valerio Orlandini" };
    MIN_RELATED		{ "index~, buffer~, groove~" };

    inlet<>  in_pb	    { this, "(signal) Sample playback increment, (number) Crossfade amount", "signal" };
    inlet<>  in_ls	    { this, "(number) Loop start", "signal" };
    inlet<>  in_le	    { this, "(number) Loop end", "signal" };

    buffer_reference m_buffer
    {
        this,
        MIN_FUNCTION
        {
            initialized_ = false;
            return {};
        }
    };

    argument<symbol> m_source_arg
    {
        this,
        "buffer",
        "buffer~ to work on."
    };

    argument<int> channels_arg
    {
        this,
        "outputs",
        "The number of output channels."
    };

    attribute<number, threadsafe::no, limit::clamp> crossfade
    {
        this,
        "crossfade",
        0.0,
        range { 0.0, 100.0 },
        title {"Crossfade (%)"},
        description {"Crossfade amount."}
    };

    attribute<number, threadsafe::no, limit::clamp> loop_start
    {
        this,
        "startloop",
        0.0,
        range { 0.0, 10000000.0 },
        title {"Loop start (ms)"},
        description {"Loop start in milliseconds."}
    };

    attribute<number, threadsafe::no, limit::clamp> loop_end
    {
        this,
        "endloop",
        0.0,
        range { 0.0, 10000000.0 },
        title {"Loop end (ms)"},
        description {"Loop end in milliseconds."}
    };

    message<> m_number
    {
        this,
        "number",
        "Set the crossfade amount or loop start or end time, according to the inlet",
        MIN_FUNCTION {
            switch (inlet)
            {
            case 0:
                crossfade = number(args[0]);
                loop_points_changed = true;
                break;
            case 1:
                target_loop_start = number(args[0]);
                loop_points_changed = true;
                break;
            case 2:
                target_loop_end = number(args[0]);
                loop_points_changed = true;
                break;
            }
            return {};
        }
    };


    crossloop_tilde(const atoms& args = {})
    {
        outputs_ = 2;

        if (!args.empty())
        {
            m_buffer.set(args[0]);
        }

        if (!args.empty() && args.size() > 1)
        {
            outputs_ = std::max(1, int(args[1])) + 1;
        }

        playhead_ = 0.0;

        for (auto o = 0; o < outputs_; ++o)
        {
            if (o < outputs_ - 1)
            {
                m_outlets.push_back(std::make_unique<outlet<>>(this, "(signal) Channel " + std::to_string(o + 1) + " output", "signal"));
            }
            else
            {
                m_outlets.push_back(std::make_unique<outlet<>>(this, "(signal) Loop sync output", "signal"));
            }
        }
    }

    void operator()(audio_bundle input, audio_bundle output)
    {
        buffer_lock<> b(m_buffer);
        if (!b.valid())
        {
            for (auto i = 0; i < input.frame_count(); ++i)
            {
                for (auto ch = 0; ch < outputs_ - 1; ch++)
                {
                    output.samples(ch)[i] = 0.0;
                }
            }
        }
        else
        {
            if (loop_points_changed.load() || !initialized_)
            {
                initialized_ = true;
                loop_start = target_loop_start.load();
                loop_end = target_loop_end.load();

                sample_start = std::floor(loop_start * b.samplerate() * 0.001);
                sample_start = std::clamp(sample_start, 0.0, static_cast<sample>(b.frame_count() - 1));

                sample_end = static_cast<sample>(b.frame_count() - 1);
                if (loop_end > 0.0)
                {
                    sample_end = std::floor(loop_end * b.samplerate() * 0.001);
                    sample_end = std::clamp(sample_end, 0.0, static_cast<sample>(b.frame_count() - 1));
                }

                if (sample_start > sample_end)
                {
                    std::swap(sample_start, sample_end);
                    direction_ = -1.0;
                }
                else
                {
                    direction_ = 1.0;
                }

                length = sample_end - sample_start;

                crossfade_samples = std::clamp(crossfade * 0.005 * length, 0.0, length * 0.5);
                crossfade_start_sample = sample_end - crossfade_samples;
                crossfade_start_sample_reversed = sample_start + crossfade_samples;

                playhead_ = std::clamp(playhead_, sample_start, sample_end);

                loop_points_changed = false;
            }

            if (length > 0.0)
            {
                for (auto i = 0; i < input.frame_count(); ++i)
                {
                    sample step = input.samples(0)[i] * direction_;
                    playhead_ += step;

                    if (step < 0.0)
                    {
                        if (playhead_ < sample_start)
                        {
                            playhead_ = sample_end - std::fmod(sample_start - playhead_, length) - crossfade_samples;
                        }
                    }
                    else
                    {
                        if (playhead_ > sample_end)
                        {
                            playhead_ = sample_start + std::fmod(playhead_ - sample_start, length) + crossfade_samples;
                        }
                    }


                    for (auto ch = 0; ch < outputs_ - 1; ch++)
                    {
                        output.samples(ch)[i] = 0.0;

                        if (ch < b.channel_count())
                        {
                            if (playhead_ > crossfade_start_sample && crossfade > 0.0 && step > 0.0)
                            {
                                sample fade_in_gain = (playhead_ - crossfade_start_sample) / crossfade_samples;
                                sample fade_out_gain = 1.0 - fade_in_gain;
                                fade_in_gain = std::sqrt(fade_in_gain);
                                fade_out_gain = std::sqrt(fade_out_gain);

                                sample ceil = std::ceil(playhead_);
                                if (ceil >= sample_end)
                                {
                                    ceil = 0.0;
                                }
                                sample bs = b.lookup(static_cast<unsigned int>(ceil), ch);
                                sample floor;
                                sample t = std::modf(playhead_, &floor);
                                sample as = b.lookup(static_cast<unsigned int>(floor), ch);
                                sample out = cosip(as, bs, t) * fade_out_gain;

                                sample corresponding_ph = sample_start + playhead_ - crossfade_start_sample;

                                ceil = std::ceil(corresponding_ph);
                                if (ceil >= sample_end)
                                {
                                    ceil = 0.0;
                                }
                                bs = b.lookup(static_cast<unsigned int>(ceil), ch);
                                floor = 0.0;
                                t = std::modf(corresponding_ph, &floor);
                                as = b.lookup(static_cast<unsigned int>(floor), ch);
                                out += cosip(as, bs, t) * fade_in_gain;

                                output.samples(ch)[i] = out;
                            }
                            else if (playhead_ < crossfade_start_sample_reversed && crossfade > 0.0 && step < 0.0)
                            {
                                sample fade_in_gain = (crossfade_start_sample_reversed - playhead_) / crossfade_samples;
                                sample fade_out_gain = 1.0 - fade_in_gain;
                                fade_in_gain = std::sqrt(fade_in_gain);
                                fade_out_gain = std::sqrt(fade_out_gain);

                                sample ceil = std::ceil(playhead_);
                                if (ceil >= sample_end)
                                {
                                    ceil = 0.0;
                                }
                                sample bs = b.lookup(static_cast<unsigned int>(ceil), ch);
                                sample floor = 0.0;
                                sample t = std::modf(playhead_, &floor);
                                sample as = b.lookup(static_cast<unsigned int>(floor), ch);
                                sample out = cosip(as, bs, t) * fade_out_gain;

                                sample corresponding_ph = sample_end - (crossfade_start_sample_reversed - playhead_);

                                ceil = std::ceil(corresponding_ph);
                                if (ceil >= sample_end)
                                {
                                    ceil = 0.0;
                                }
                                bs = b.lookup(static_cast<unsigned int>(ceil), ch);
                                floor;
                                t = std::modf(corresponding_ph, &floor);
                                as = b.lookup(static_cast<unsigned int>(floor), ch);
                                out += cosip(as, bs, t) * fade_in_gain;

                                output.samples(ch)[i] = out;
                            }
                            else
                            {
                                sample ceil = std::ceil(playhead_);
                                if (ceil >= sample_end)
                                {
                                    ceil = 0.0;
                                }
                                sample bs = b.lookup(static_cast<unsigned int>(ceil), ch);
                                sample floor;
                                sample t = std::modf(playhead_, &floor);
                                sample as = b.lookup(static_cast<unsigned int>(floor), ch);

                                output.samples(ch)[i] = cosip(as, bs, t);
                            }
                        }
                    }
                }
            }
            else
            {
                for (auto i = 0; i < input.frame_count(); ++i)
                {
                    for (auto ch = 0; ch < outputs_ - 1; ch++)
                    {
                        output.samples(ch)[i] = 0.0;
                    }
                }
            }
        }
    }

private:
    int outputs_;
    std::vector<std::unique_ptr<outlet<>>> m_outlets;
    sample playhead_{0.0};
    sample direction_{1.0};
    std::atomic<bool> loop_points_changed{false};
    std::atomic<sample> target_loop_start{0.0};
    std::atomic<sample> target_loop_end{0.0};
    sample sample_start{0.0};
    sample sample_end{0.0};
    sample length{0.0};
    sample crossfade_samples{0.0};
    sample crossfade_start_sample{0.0};
    sample crossfade_start_sample_reversed{0.0};
    bool initialized_{false};
};

MIN_EXTERNAL(crossloop_tilde);



