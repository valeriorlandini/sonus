/// @file       sonus.buffx.cpp
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <random>
#include "../include/biquad.h"

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
            buffer_lock<> b(m_buffer);

            if (b.valid() && original_buffer_.size() > 0)
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
                    std::vector<double> reversed_channel(b.frame_count(), 0.0);
                    
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

    message<> lowpass
    {
        this,
        "lowpass",
        "Apply a lowpass filter: lowpass freq q",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: lowpass <cutoff frequency> <resonance [optional, defaults to 0.707]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double cutoff = double(args.at(0));
                double q = 0.707;

                if (args.size() > 1)
                {
                    q = double(args.at(1));
                }

                Biquad<double> filter(b.samplerate(), cutoff, q, LOWPASS);

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = filter.run(b.lookup(s, ch));
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };
    
    message<> hipass
    {
        this,
        "hipass",
        "Apply a highpass filter: hipass freq q",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: hipass <cutoff frequency> <resonance [optional, defaults to 0.707]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double cutoff = double(args.at(0));
                double q = 0.707;

                if (args.size() > 1)
                {
                    q = double(args.at(1));
                }

                Biquad<double> filter(b.samplerate(), cutoff, q, HIPASS);

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = filter.run(b.lookup(s, ch));
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };

    message<> bandpass
    {
        this,
        "bandpass",
        "Apply a bandpass filter: bandpass freq q",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: hipass <cutoff frequency> <resonance [optional, defaults to 0.707]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double cutoff = double(args.at(0));
                double q = 0.707;

                if (args.size() > 1)
                {
                    q = double(args.at(1));
                }

                Biquad<double> filter(b.samplerate(), cutoff, q, BANDPASS);

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = filter.run(b.lookup(s, ch));
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };
    
    message<> notch
    {
        this,
        "notch",
        "Apply a notch filter: notch freq q",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: notch <cutoff frequency> <resonance [optional, defaults to 0.707]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double cutoff = double(args.at(0));
                double q = 0.707;

                if (args.size() > 1)
                {
                    q = double(args.at(1));
                }

                Biquad<double> filter(b.samplerate(), cutoff, q, BANDREJECT);

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = filter.run(b.lookup(s, ch));
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };
    
    message<> allpass
    {
        this,
        "allpass",
        "Apply an allpass filter: allpass freq q",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: allpass <cutoff frequency> <resonance [optional, defaults to 0.707]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double cutoff = double(args.at(0));
                double q = 0.707;

                if (args.size() > 1)
                {
                    q = double(args.at(1));
                }

                Biquad<double> filter(b.samplerate(), cutoff, q, ALLPASS);

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = filter.run(b.lookup(s, ch));
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };
    
    

private:
    std::vector<std::vector<double>> original_buffer_;

    void save_buffer()
    {
        buffer_lock<> b(m_buffer);

        if (b.valid())
        {
            original_buffer_.clear();

            for (int ch = 0; ch < b.channel_count(); ch++)
            {
                std::vector<double> curr_channel;

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
