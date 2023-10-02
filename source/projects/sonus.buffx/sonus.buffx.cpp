/// @file       sonus.buffx.cpp
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <random>
#include "../include/interp.h"

using namespace c74::min;

class buffx : public object<buffx>
{
public:
    MIN_DESCRIPTION	{ "Apply effects to a buffer~." };
    MIN_TAGS		{ "audio, sampling" };
    MIN_AUTHOR		{ "Valerio Orlandini" };
    MIN_RELATED		{ "index~, buffer~" };

    inlet<>  in	    { this, "Effect to apply with the requested parameters" };
    outlet<> out	{ this, "(symbol) Notification that the content of the buffer~ changed" };

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
            buffer_name = arg;
        }
    };

    attribute<bool> backup
    {
        this,
        "backup",
        false,
        title {"Backup current buffer"},
        description {"Whether to save or not a copy of the current buffer content."},
        setter
        {
            MIN_FUNCTION
            {
                bool n = args[0];
                if (n)
                {
                    save_buffer();
                }
                return {n};
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
            buffer_lock<> b(m_buffer);

            if (b.valid() && backup && original_buffer_.size() > 0)
            {
                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    if (original_buffer_.size() > ch)
                    {
                        for (int s = 0; s < b.frame_count(); s++)
                        {
                            if (original_buffer_.at(ch).size() > s)
                            {
                                b.lookup(s, ch) = original_buffer_.at(ch).at(s);
                            }
                        }
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
        "Reverse the content of the buffer",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    std::vector<float> reversed_channel(b.frame_count(), 0.0);
                    
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        reversed_channel.at(b.frame_count() - s - 1) = b.lookup(s, ch);
                    }

                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = reversed_channel.at(s);
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };
    
    

private:
    std::string buffer_name;
    std::vector<std::vector<float>> original_buffer_;

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

MIN_EXTERNAL(buffx);
