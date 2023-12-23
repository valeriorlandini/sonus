/// @file       sonus.wavesets.cpp
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <random>
#include "interp.h"

using namespace c74::min;
using namespace soutel;

template <class TSample>
struct waveset_params
{
    unsigned int start;
    unsigned int half;
    unsigned int end;
    TSample peak;
};

enum Stages
{
    BEGIN,
    HALF
};

enum Shapes
{
    SINE,
    SAW,
    PULSE,
    TRIANGLE
};

class wavesets : public object<wavesets>
{
public:
    MIN_DESCRIPTION	{ "Make wavesets operations a buffer~." };
    MIN_TAGS		{ "audio, sampling" };
    MIN_AUTHOR		{ "Valerio Orlandini" };
    MIN_RELATED		{ "index~, buffer~, wave~, sonus.buffx" };

    inlet<>  in	    { this, "Buffer transformation based on wavesets detection" };
    outlet<> out	{ this, "(symbol) Information about the detected wavesets" };

    buffer_reference m_buffer
    {
        this,
        MIN_FUNCTION
        {
            return {};
        }
    };

    argument<symbol> m_source_arg
    {
        this,
        "buffer",
        "buffer~ to transform.",
        MIN_ARGUMENT_FUNCTION
        {
            m_buffer.set(arg);
        }
    };

    message<> backup
    {
        this,
        "backup",
        "Backup the current buffer content",
        setter
        {
            MIN_FUNCTION
            {
                save_buffer();
                return {};
            }
        }
    };

    message<> restore
    {
        this,
        "restore",
        "Restore the buffer content at the time when backup was set",
        MIN_FUNCTION
        {
            buffer_lock<false> b(m_buffer);

            if (b.valid() && original_buffer_.size() > 0)
            {
                b.resize_in_samples(original_buffer_.at(0).size());

                b.dirty();

                buffer_lock<> b_new(m_buffer);

                if (b_new.valid())
                {
                    for (auto ch = 0; ch < b_new.channel_count(); ch++)
                    {
                        if (original_buffer_.size() > ch)
                        {
                            for (auto s = 0; s < b_new.frame_count(); s++)
                            {
                                if (original_buffer_.at(ch).size() > s)
                                {
                                    b_new.lookup(s, ch) = original_buffer_.at(ch).at(s);
                                }
                            }
                        }
                    }

                    b_new.dirty();
                }
            }
            
            return {};
        }
    };

    message<> mute
    {
        this,
        "mute",
        "Mute n wavesets after keeping m",
        MIN_FUNCTION
        {
            if (!args.empty())
            {
                unsigned int kept = 1;
                unsigned int mute = 0;

                if (args.size() >= 2)
                {
                    kept = int(args.at(0));
                    mute = int(args.at(1));
                }
                else
                {
                    mute = int(args.at(0));
                }

                analyse();
                buffer_lock<> b(m_buffer);

                if (b.valid() && (kept + mute))
                {
                    for (int ch = 0; ch < b.channel_count(); ++ch)
                    {
                        for (int w = 0; w < wavesets_idx_.at(ch).size(); ++w)
                        {
                            if (w % (kept + mute) >= kept)
                            {
                                int start = wavesets_idx_.at(ch).at(w).start;
                                int end = wavesets_idx_.at(ch).at(w).end;

                                for (int s = start; s <= end; s++)
                                {
                                    b.lookup(s, ch) = 0.0;
                                }
                            }
                        }
                    }

                    b.dirty();
                }
            }            

            return {};
        }
    };

    message<> shuffle
    {
        this,
        "shuffle",
        "Randomly reorder the wavesets, optionally in groups of n",
        MIN_FUNCTION
        {
            analyse();
            buffer_lock<> b(m_buffer);

            int group_size = 1;

            if (!args.empty())
            {
                group_size = std::max(1, int(args.at(0)));
            }

            if (b.valid())
            {
                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    int groups = std::max(1, int(ceil((float)wavesets_idx_.at(ch).size() / (float)group_size)));
                    if (groups == 1)
                    {
                        continue;
                    }

                    auto rand_dev = std::random_device {}; 
                    auto rand_eng = std::default_random_engine {rand_dev()};
                    std::vector<int> ws(groups);
                    std::iota(std::begin(ws), std::end(ws), 0);
                    std::shuffle(std::begin(ws), std::end(ws), rand_eng);
                    std::vector<float> new_wave;

                    for (int g = 0; g < groups; g++)
                    {
                        int ws_begin = ws.at(g) * group_size;
                        int ws_end = std::min(int(wavesets_idx_.at(ch).size()), (ws.at(g) + 1) * group_size) - 1;
                        
                        int start = wavesets_idx_.at(ch).at(ws_begin).start;
                        int end = wavesets_idx_.at(ch).at(ws_end).end;
                       
                        for (int s = start; s <= end; ++s)
                        {
                            new_wave.push_back(b.lookup(s, ch));
                        }
                    }

                    for (int n = 0; n < new_wave.size(); n++)
                    {
                        b.lookup(n, ch) = new_wave.at(n);
                    }
                }

                b.dirty();
            }

            return {};
        }
    };

    message<> reverse
    {
        this,
        "reverse",
        "Reverse each or a group of n wavesets",
        MIN_FUNCTION
        {
            analyse();
            buffer_lock<> b(m_buffer);

            int rev_group = 1;

            if (!args.empty())
            {
                rev_group = std::max(1, int(args.at(0)));
            }

            if (b.valid())
            {
                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    int w = 0;
                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += rev_group)
                    {
                        int start = wavesets_idx_.at(ch).at(w).start;
                        int end = wavesets_idx_.at(ch).at(std::min(int(wavesets_idx_.at(ch).size()) - 1, w + rev_group - 1)).end;
                        
                        std::vector<float> curr_waveset;

                        for (int s = end; s >= start; --s)
                        {
                            curr_waveset.push_back((float)b.lookup(s, ch));
                        }
     
                        int pos = 0;

                        for (int s = start; s <= end; ++s)
                        {
                            b.lookup(s, ch) = curr_waveset.at(pos++);
                        }
                    }

                    if (w - rev_group < wavesets_idx_.at(ch).size() - 1)
                    {
                        int start = wavesets_idx_.at(ch).at(w - rev_group).start;
                        int end = wavesets_idx_.at(ch).at(int(wavesets_idx_.at(ch).size()) - 1).end;
                        
                        std::vector<float> curr_waveset;

                        for (int s = end; s >= start; --s)
                        {
                            curr_waveset.push_back((float)b.lookup(s, ch));
                        }
     
                        int pos = 0;

                        for (int s = start; s <= end; ++s)
                        {
                            b.lookup(s, ch) = curr_waveset.at(pos++);
                        }
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };

    message<> average
    {
        this,
        "average",
        "Average each group of wavesets with the next one",
        MIN_FUNCTION
        {
            analyse();
            buffer_lock<> b(m_buffer);

            int avg_group = 1;

            if (!args.empty())
            {
                avg_group = std::max(1, int(args.at(0)));
            }


            if (b.valid())
            {
                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    int w = 0;
                    std::vector<float> new_buffer;
                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += avg_group)
                    {
                        int start_curr = wavesets_idx_.at(ch).at(w).start;
                        int end_curr = wavesets_idx_.at(ch).at((w + avg_group - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        int start_next = wavesets_idx_.at(ch).at((w + avg_group) % wavesets_idx_.at(ch).size()).start;
                        int end_next = wavesets_idx_.at(ch).at((w + (avg_group * 2) - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        std::vector<float> curr_waveset;
                        std::vector<float> next_waveset;

                        if (end_curr < start_curr)
                        {
                            end_curr += b.frame_count();
                        }
                        for (int s = start_curr; s <= end_curr; ++s)
                        {
                            curr_waveset.push_back((float)b.lookup(s % b.frame_count(), ch));
                        }

                        if (end_next < start_next)
                        {
                            end_next += b.frame_count();
                        }
                        for (int s = start_next; s <= end_next; ++s)
                        {
                            next_waveset.push_back((float)b.lookup(s % b.frame_count(), ch));
                        }
     
                        std::vector<float> resized_next = resize_chunk(next_waveset, curr_waveset.size());

                        for (int s = 0; s < curr_waveset.size(); s++)
                        {
                            new_buffer.push_back((curr_waveset.at(s) + resized_next.at(s)) * 0.5);
                        }
                    }
                    for (int s = 0; s < new_buffer.size(); s++)
                    {
                        b.lookup(s, ch) = new_buffer.at(s);
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };
    
    message<> multiply
    {
        this,
        "multiply",
        "Multiply each group of wavesets with the next one",
        MIN_FUNCTION
        {
            analyse();
            buffer_lock<> b(m_buffer);

            int avg_group = 1;

            if (!args.empty())
            {
                avg_group = std::max(1, int(args.at(0)));
            }


            if (b.valid())
            {
                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    int w = 0;
                    std::vector<float> new_buffer;
                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += avg_group)
                    {
                        int start_curr = wavesets_idx_.at(ch).at(w).start;
                        int end_curr = wavesets_idx_.at(ch).at((w + avg_group - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        int start_next = wavesets_idx_.at(ch).at((w + avg_group) % wavesets_idx_.at(ch).size()).start;
                        int end_next = wavesets_idx_.at(ch).at((w + (avg_group * 2) - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        std::vector<float> curr_waveset;
                        std::vector<float> next_waveset;

                        if (end_curr < start_curr)
                        {
                            end_curr += b.frame_count();
                        }
                        for (int s = start_curr; s <= end_curr; ++s)
                        {
                            curr_waveset.push_back((float)b.lookup(s % b.frame_count(), ch));
                        }

                        if (end_next < start_next)
                        {
                            end_next += b.frame_count();
                        }
                        for (int s = start_next; s <= end_next; ++s)
                        {
                            next_waveset.push_back((float)b.lookup(s % b.frame_count(), ch));
                        }
     
                        std::vector<float> resized_next = resize_chunk(next_waveset, curr_waveset.size());

                        for (int s = 0; s < curr_waveset.size(); s++)
                        {
                            new_buffer.push_back(curr_waveset.at(s) * resized_next.at(s));
                        }
                    }
                    for (int s = 0; s < new_buffer.size(); s++)
                    {
                        b.lookup(s, ch) = new_buffer.at(s);
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };
    
    message<> power
    {
        this,
        "power",
        "Raise each sample of a group of wavesets with the absolute value of the samples of the next one as exponent",
        MIN_FUNCTION
        {
            analyse();
            buffer_lock<> b(m_buffer);

            int avg_group = 1;

            if (!args.empty())
            {
                avg_group = std::max(1, int(args.at(0)));
            }


            if (b.valid())
            {
                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    int w = 0;
                    std::vector<float> new_buffer;
                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += avg_group)
                    {
                        int start_curr = wavesets_idx_.at(ch).at(w).start;
                        int end_curr = wavesets_idx_.at(ch).at((w + avg_group - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        int start_next = wavesets_idx_.at(ch).at((w + avg_group) % wavesets_idx_.at(ch).size()).start;
                        int end_next = wavesets_idx_.at(ch).at((w + (avg_group * 2) - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        std::vector<float> curr_waveset;
                        std::vector<float> next_waveset;

                        if (end_curr < start_curr)
                        {
                            end_curr += b.frame_count();
                        }
                        for (int s = start_curr; s <= end_curr; ++s)
                        {
                            curr_waveset.push_back((float)b.lookup(s % b.frame_count(), ch));
                        }

                        if (end_next < start_next)
                        {
                            end_next += b.frame_count();
                        }
                        for (int s = start_next; s <= end_next; ++s)
                        {
                            next_waveset.push_back((float)b.lookup(s % b.frame_count(), ch));
                        }
     
                        std::vector<float> resized_next = resize_chunk(next_waveset, curr_waveset.size());

                        for (int s = 0; s < curr_waveset.size(); s++)
                        {
                            new_buffer.push_back(std::copysign(0.5, curr_waveset.at(s)) * std::pow(std::abs(curr_waveset.at(s)), std::abs(resized_next.at(s))));
                        }
                    }
                    for (int s = 0; s < new_buffer.size(); s++)
                    {
                        b.lookup(s, ch) = new_buffer.at(s);
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };

    message<> reshape
    {
        this,
        "reshape",
        "Transform all the wavesets in the desired shape (sin, saw, tri, rect)",
        MIN_FUNCTION
        {
            if (!args.empty())
            {
                analyse();
                buffer_lock<> b(m_buffer);
                Shapes waveshape = SINE;
                if (args[0] == "sin")
                {
                    waveshape = SINE;
                }
                else if (args[0] == "saw")
                {
                    waveshape = SAW;
                }
                else if (args[0] == "rect")
                {
                    waveshape = PULSE;
                }
                else if (args[0] == "tri")
                {
                    waveshape = TRIANGLE;
                }
                else
                {
                    cout << "Unknown shape, defaulting to sine" << endl;
                }

                if (b.valid())
                {
                    for (int ch = 0; ch < b.channel_count(); ch++)
                    {
                        for (int w = 0; w < wavesets_idx_.at(ch).size(); w++)
                        {
                            float wave_length = wavesets_idx_.at(ch).at(w).end - wavesets_idx_.at(ch).at(w).start;

                            if (wave_length > 0.0)
                            {
                                for (int s = wavesets_idx_.at(ch).at(w).start; s <= wavesets_idx_.at(ch).at(w).end; s++)
                                {
                                    float ramp = -1.0 * wrap(((wavesets_idx_.at(ch).at(w).end - s) / wave_length) * 2.0, -1.0, 1.0);
                                    if (ramp == -0.0)
                                    {
                                        ramp = 0.0;
                                    }
                                    switch (waveshape)
                                    {
                                        case SAW:
                                            b.lookup(s, ch) = ramp;
                                            break;
                                        case SINE:
                                            b.lookup(s, ch) = sin(M_PI * ramp);
                                            break;
                                        case TRIANGLE:
                                            if (fabs(ramp) >= 0.0 && fabs(ramp) <= 0.5)
                                            {
                                                b.lookup(s, ch) = ramp * 2.0;
                                            }
                                            else if (ramp > 0.5)
                                            {
                                                b.lookup(s, ch) = (1.0 - ramp) * 2.0;
                                            }
                                            else if (ramp < -0.5)
                                            {
                                                b.lookup(s, ch) = (1.0 + ramp) * -2.0;
                                            }
                                            break;
                                        case PULSE:
                                            if (s < wavesets_idx_.at(ch).at(w).half)
                                            {
                                                b.lookup(s, ch) = -1.0;
                                            }
                                            else
                                            {
                                                b.lookup(s, ch) = 1.0;
                                            }
                                            break;
                                    }

                                    b.lookup(s, ch) *= wavesets_idx_.at(ch).at(w).peak;
                                }
                            }
                        }
                    }

                    b.dirty();
                }
            }
            else
            {
                cout << "Missing argument" << endl;
            }       

            return {};

        }
    };

private:
    std::vector<std::vector<waveset_params<float>>> wavesets_idx_;
    std::vector<std::vector<float>> original_buffer_;

    void analyse()
    {
        buffer_lock<> b(m_buffer);

        if (b.valid())
        {
            wavesets_idx_.clear();

            for (int ch = 0; ch < b.channel_count(); ch++)
            {
                std::vector<waveset_params<float>> curr_channel;
                waveset_params<float> curr_waveset;
                bool sign;
                Stages stage = BEGIN;
                curr_waveset.start = 0;
                curr_waveset.half = 0;
                curr_waveset.peak = 0.0;
                for (int s = 0; s < b.frame_count(); s++)
                {
                    if (!s)
                    {
                        sign = std::signbit(b.lookup(s, ch));
                    }
                    else if (std::signbit(b.lookup(s, ch)) != sign)
                    {
                        if (stage == BEGIN)
                        {
                            curr_waveset.half = s;
                            stage = HALF;
                        }
                    else
                    {
                        curr_waveset.end = std::max(0, s - 1);
                        curr_channel.push_back(curr_waveset);
                        curr_waveset.start = s;
                        curr_waveset.half = s;
                        curr_waveset.peak = 0.0;
                        stage = BEGIN;
                    }
                }

                    sign = std::signbit(b.lookup(s, ch));

                    if (fabs(b.lookup(s, ch)) > curr_waveset.peak)
                    {
                        curr_waveset.peak = (float)fabs(b.lookup(s, ch));
                    }
                }
                curr_waveset.end = b.frame_count() - 1;
                curr_channel.push_back(curr_waveset);
                wavesets_idx_.push_back(curr_channel);
            }

            for (auto c = 0; c < wavesets_idx_.size(); c++)
            {
                out.send("Channel " + std::to_string(c + 1) + " wavesets: " + std::to_string(wavesets_idx_.at(c).size()));
            }
        }
    }

    void save_buffer()
    {
        buffer_lock<> b(m_buffer);

        if (b.valid())
        {
            original_buffer_.clear();

            for (int ch = 0; ch < b.channel_count(); ch++)
            {
                std::vector<float> curr_channel;

                for (int s = 0; s < b.frame_count(); s++)
                {
                    curr_channel.push_back(b.lookup(s, ch));
                }

                original_buffer_.push_back(curr_channel);
            }
        }
    }
};

MIN_EXTERNAL(wavesets);
