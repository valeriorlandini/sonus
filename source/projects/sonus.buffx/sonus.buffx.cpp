/// @file       sonus.buffx.cpp
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <random>
#include "../include/biquad.h"
#include "../include/cryptoverb.h"
#include "../include/delay.h"
#include "../include/distortions.h"

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
        "Apply a lowpass filter: lowpass [freq] [q]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: lowpass <cutoff frequency> <resonance [optional, default 0.707]>" << endl;

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
        "Apply a highpass filter: hipass [freq] [q]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: hipass <cutoff frequency> <resonance [optional, default 0.707]>" << endl;

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
        "Apply a bandpass filter: bandpass [freq] [q]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: hipass <cutoff frequency> <resonance [optional, default 0.707]>" << endl;

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
        "Apply a notch filter: notch [freq] [q]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: notch <cutoff frequency> <resonance [optional, default 0.707]>" << endl;

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
        "Apply an allpass filter: allpass [freq] [q]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: allpass <cutoff frequency> <resonance [optional, default 0.707]>" << endl;

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
    
    message<> noise
    {
        this,
        "noise",
        "Fill the buffer with noise, optionaly mixing it with current content: noise [mix]",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                std::default_random_engine generator;
                std::uniform_real_distribution<double> distribution(-1.0, 1.0);
                
                double mix = 1.0;

                if (args.size() > 0)
                {
                    mix = std::clamp(double(args.at(0)), 0.0, 1.0);
                }

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        double number = distribution(generator);
                        b.lookup(s, ch) = (number * mix) + (b.lookup(s, ch) * (1.0 - mix));
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };

    message<> drive
    {
        this,
        "drive",
        "Apply a symmetrical soft clipping: drive [threshold]",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            if (b.valid())
            {                
                double threshold = 1.0 / 3.0;

                if (args.size() > 0)
                {
                    threshold = std::clamp(double(args.at(0)), 0.0, 1.0);
                }

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = symmetrical_soft_clip(double(b.lookup(s, ch)), threshold);
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };    
    
    message<> distort
    {
        this,
        "distort",
        "Apply an exponential distortion: distort [gain] [wet]",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            if (args.size() < 1)
            {
                cerr << "Syntax: distort <gain> <wet [optional, default 1.0]>" << endl;

                return {};
            }

            if (b.valid())
            {
                double gain = double(args.at(0));
                double wet = 1.0;

                if (args.size() > 1)
                {
                    wet = std::clamp(double(args.at(1)), 0.0, 1.0);
                }

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = exponential_distortion(double(b.lookup(s, ch)), gain, wet);
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };    

    message<> bits
    {
        this,
        "bits",
        "Apply a bitcrusher effect: bits [bit depth] [wet]",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            if (args.size() < 1)
            {
                cerr << "Syntax: bits <bit depth> <wet [optional, default 1.0]>" << endl;

                return {};
            }

            if (b.valid())
            {
                double bit_depth = std::clamp(double(args.at(0)), 1.0, 64.0);
                double wet = 1.0;

                if (args.size() > 1)
                {
                    wet = std::clamp(double(args.at(1)), 0.0, 1.0);
                }

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = bitcrush(double(b.lookup(s, ch)), bit_depth, wet);
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };    
    
    message<> delay
    {
        this,
        "delay",
        "Apply a delay: delay [time in milliseconds] [feedback]",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            if (args.size() < 1)
            {
                cerr << "Syntax: mix <time in milliseconds> <feedback [optional, default 0.0]>" << endl;

                return {};
            }

            double delay_time = std::clamp(double(args.at(0)), 0.0, 30000.0);
            double feedback = 0.0;
            double wet = 0.25;

            if (args.size() > 1)
            {
                feedback = std::clamp(double(args.at(1)), -1.0, 1.0);
            }

            if (args.size() > 2)
            {
                wet = std::clamp(double(args.at(2)), 0.0, 1.0);
            }

            if (b.valid())
            {
                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    Delay<double> delay(b.samplerate(), delay_time);
                    delay.set_feedback(feedback);
                    
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) += delay.run(double(b.lookup(s, ch))) * wet;
                    }
                }

                b.dirty();
            }        

            return {};
        }
    };

    message<> fade
    {
        this,
        "fade",
        "Fade in or out: fade [in / out] [time] [ms / %]",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            if (args.size() < 2)
            {
                cerr << "Syntax: fade <in / out> <time> <time unit: ms or %, default ms>" << endl;

                return {};
            }

            bool fade_out = (args.at(0) == "out");
            double fade_time = double(args.at(1));
            bool ms = true;

            if (args.size() > 2)
            {
                ms = (args.at(2) == "ms");
            }

            if (b.valid())
            {
                unsigned int fade_samples = 0;

                if (ms)
                {
                    fade_time = std::clamp(fade_time, 0.0, 1000.0 * b.frame_count() / b.samplerate());
                    fade_samples = (unsigned int)(floor(fade_time * b.samplerate() * 0.001));
                }
                else
                {
                    fade_time = std::clamp(fade_time, 0.0, 100.0);
                    fade_samples = (unsigned int)(floor(b.frame_count() * fade_time * 0.01));
                }

                fade_samples = std::clamp(fade_samples, (unsigned int)0, (unsigned int)(b.frame_count() - 1));

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    if (fade_samples > 0)
                    {
                        if (!fade_out)
                        {
                            for (auto s = 0; s < fade_samples; s++)
                            {
                                b.lookup(s, ch) *= std::pow((double)s / (double)fade_samples, 2.0);
                            }
                        }
                        else
                        {
                            double target = (double)(b.frame_count() - 1 - fade_samples);
                            for (auto s = b.frame_count() - 1; s > b.frame_count() - 1 - fade_samples; s--)
                            {
                                b.lookup(s, ch) *= std::pow((double)(b.frame_count() - 1 - s) / (double)fade_samples, 2.0);
                            }
                        }
                    }    
                }

                b.dirty();
            }        

            return {};
        }
    };    

    message<> mix
    {
        this,
        "mix",
        "Mix with other buffers: mix [buffer name] [buffer gain] ... [buffer name] [buffer gain]",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            if (args.size() < 1 || args.size() % 2 != 0)
            {
                cerr << "Syntax: mix <buffer name> <buffer gain> ... <buffer name> <buffer gain>" << endl;

                return {};
            }


            if (b.valid())
            {
                for (auto i = 0; i < args.size(); i += 2)
                {
                    buffer_reference buffer(this, nullptr, false);
                    buffer.set(args.at(i));
                    buffer_lock bl(buffer);
                    if (bl.valid())
                    {
                        double gain = double(args.at(i+1));
                        auto length = std::min(bl.frame_count(), b.frame_count());
                        auto channels = std::min(bl.channel_count(), b.channel_count());

                        for (auto ch = 0; ch < channels; ch++)
                        {
                            for (auto s = 0; s < length; s++)
                            {
                                b.lookup(s, ch) += bl.lookup(s, ch) * gain;
                            }
                        }

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
