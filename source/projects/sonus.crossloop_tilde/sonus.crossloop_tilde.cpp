/// @file       sonus.crossloop_tilde.cpp
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <cmath>
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

    inlet<>  in_pb	    { this, "(signal) Sample playback increment", "signal" };
    inlet<>  in_ls	    { this, "(signal/float) Loop start", "signal" };
    inlet<>  in_le	    { this, "(signal/float) Loop end", "signal" };

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
        "buffer~ to work on.",
        MIN_ARGUMENT_FUNCTION
        {
            m_buffer.set(arg);
        }
    };

	argument<int> channels_arg
    {
        this,
        "inputs",
        "The number of output channels."
    };

    crossloop_tilde(const atoms& args = {})
    {
        outputs_ = 2;

        if (!args.empty() && args.size() > 1)
        {
            outputs_ = std::max(1, int(args[1])) + 1;
        }

        playhead_.resize(outputs_ - 1, 0.0);

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

        for (auto i = 0; i < input.frame_count(); ++i)
        {
            for (auto ch = 0; ch < outputs_ - 1; ch++)
            {
                if (b.valid() && ch < b.channel_count())
                {
                    playhead_[ch] += input.samples(0)[ch];

                    while (playhead_[ch] < 0.0)
                    {
                        playhead_[ch] += b.frame_count();
                    }

                    if (playhead_[ch] >= b.frame_count())
                    {
                        playhead_[ch] = std::fmod(playhead_[ch], b.frame_count());
                    }

                    sample ceil = std::ceil(playhead_[ch]);
                    if (ceil >= b.frame_count())
                    {
                        ceil = 0.0;
                    }
                    sample bs = b.lookup(static_cast<unsigned int>(ceil), ch);
                    sample floor;
                    sample t = std::modf(playhead_[ch], &floor);
                    sample as = b.lookup(static_cast<unsigned int>(floor), ch);

                    output.samples(ch)[i] = cosip(as, bs, t);
                }
                else
                {
                    output.samples(ch)[i] = 0.0;
                }
            }
	    }
    }

private:
    int outputs_;
    std::vector<std::unique_ptr<outlet<>>> m_outlets;
    std::vector<sample> playhead_;
};

MIN_EXTERNAL(crossloop_tilde);
