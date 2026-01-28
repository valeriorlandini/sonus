/// @file       sonus.buffx.cpp
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <functional>
#include <random>
#include "biquad.h"
#include "blosc.h"
#include "cryptoverb.h"
#include "delay.h"
#include "distortions.h"
#define DJ_FFT_IMPLEMENTATION
#include "dj_fft.h"

using namespace c74::min;
using namespace soutel;

class buffx : public object<buffx>
{
public:
    MIN_DESCRIPTION	{ "Apply effects to a buffer~." };
    MIN_TAGS		{ "audio, sampling" };
    MIN_AUTHOR		{ "Valerio Orlandini" };
    MIN_RELATED		{ "index~, buffer~, sonus.wavesets" };

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

                Biquad<double> filter(b.samplerate(), cutoff, q, 0.0, BQFilters::lowpass);

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

                Biquad<double> filter(b.samplerate(), cutoff, q, 0.0, BQFilters::hipass);

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
                cerr << "Syntax: hipass <center frequency> <resonance [optional, default 0.707]>" << endl;

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

                Biquad<double> filter(b.samplerate(), cutoff, q, 0.0, BQFilters::bandpass);

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
                cerr << "Syntax: notch <center frequency> <resonance [optional, default 0.707]>" << endl;

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

                Biquad<double> filter(b.samplerate(), cutoff, q, 0.0, BQFilters::bandreject);

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

                Biquad<double> filter(b.samplerate(), cutoff, q, 0.0, BQFilters::allpass);

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
 
    message<> lowshelf
    {
        this,
        "lowshelf",
        "Apply a low shelving filter: lowshelf [freq] [gain]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: lowshelf <cutoff frequency> <gain [optional, default 6.0]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double cutoff = double(args.at(0));
                double gain = 6.0;

                if (args.size() > 1)
                {
                    gain = double(args.at(1));
                }

                Biquad<double> filter(b.samplerate(), cutoff, 0.707, gain, BQFilters::lowshelf);

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
 
    message<> hishelf
    {
        this,
        "hishelf",
        "Apply a high shelving filter: hishelf [freq] [gain]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: hishelf <cutoff frequency> <gain [optional, default 6.0]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double cutoff = double(args.at(0));
                double gain = 6.0;

                if (args.size() > 1)
                {
                    gain = double(args.at(1));
                }

                Biquad<double> filter(b.samplerate(), cutoff, 0.707, gain, BQFilters::hishelf);

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
 
    message<> peak
    {
        this,
        "peak",
        "Apply a peak filter: peak [freq] [gain]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: peak <center frequency> <gain [optional, default 6.0]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double cutoff = double(args.at(0));
                double gain = 6.0;

                if (args.size() > 1)
                {
                    gain = double(args.at(1));
                }

                Biquad<double> filter(b.samplerate(), cutoff, 0.707, gain, BQFilters::peak);

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

    message<> rm
    {
        this,
        "rm",
        "Ring modulate the buffer with a simple wave: rm [freq] [waveform]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: rm <frequency> <waveform [optional, sine/saw/square/tri, default sine]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double freq = double(args.at(0));
                BLOsc<double> oscillator(b.samplerate(), std::clamp(freq, -0.5 * b.samplerate(), 0.5 * b.samplerate()));
                BLWaveforms waveform = BLWaveforms::sine;

                if (args.size() > 1)
                {
                    if (args.at(1) == "saw")
                    {
                        waveform = BLWaveforms::saw;
                    }
                    if (args.at(1) == "square")
                    {
                        waveform = BLWaveforms::square;
                    }
                    if (args.at(1) == "tri")
                    {
                        waveform = BLWaveforms::triangle;
                    }
                }

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    oscillator.reset();
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        oscillator.run();
                        switch (waveform)
                        {
                            case BLWaveforms::sine:
                            b.lookup(s, ch) *= oscillator.get_sine();
                            break;
                            case BLWaveforms::saw:
                            b.lookup(s, ch) *= oscillator.get_saw();
                            break;
                            case BLWaveforms::square:
                            b.lookup(s, ch) *= oscillator.get_square();
                            break;
                            case BLWaveforms::triangle:
                            b.lookup(s, ch) *= oscillator.get_triangle();
                            break;
                        }
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };

    message<> rmbuf
    {
        this,
        "rmbuf",
        "Ring modulate the buffer with another buffer: rm [buffer name]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: rmbuf <buffer name>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                buffer_reference buffer(this, nullptr, false);
                buffer.set(args.at(0));
                buffer_lock bl(buffer);
                if (bl.valid())
                {
                    auto mod_s = bl.frame_count();
                    auto mod_c = bl.channel_count();

                    for (auto ch = 0; ch < b.channel_count(); ch++)
                    {
                        for (auto s = 0; s < b.frame_count(); s++)
                        {
                            b.lookup(s, ch) *= bl.lookup(s % mod_s, ch % mod_c);
                        }
                    }

                }

                b.dirty();
            }

            return {};
        }
    };

    message<> am
    {
        this,
        "am",
        "Amplitude modulate the buffer with a simple wave: am [freq] [waveform]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: am <frequency> <waveform [optional, sine/saw/square/tri, default sine]>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                double freq = double(args.at(0));
                BLOsc<double> oscillator(b.samplerate(), std::clamp(freq, -0.5 * b.samplerate(), 0.5 * b.samplerate()));
                BLWaveforms waveform = BLWaveforms::sine;

                if (args.size() > 1)
                {
                    if (args.at(1) == "saw")
                    {
                        waveform = BLWaveforms::saw;
                    }
                    if (args.at(1) == "square")
                    {
                        waveform = BLWaveforms::square;
                    }
                    if (args.at(1) == "tri")
                    {
                        waveform = BLWaveforms::triangle;
                    }
                }

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    oscillator.reset();
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        oscillator.run();
                        switch (waveform)
                        {
                            case BLWaveforms::sine:
                            b.lookup(s, ch) *= (oscillator.get_sine() + 1.0) * 0.5;
                            break;
                            case BLWaveforms::saw:
                            b.lookup(s, ch) *= (oscillator.get_saw() + 1.0) * 0.5;
                            break;
                            case BLWaveforms::square:
                            b.lookup(s, ch) *= (oscillator.get_square() + 1.0) * 0.5;
                            break;
                            case BLWaveforms::triangle:
                            b.lookup(s, ch) *= (oscillator.get_triangle() + 1.0) * 0.5;
                            break;
                        }
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };

    message<> ambuf
    {
        this,
        "ambuf",
        "Amplitude modulate the buffer with another buffer: am [buffer name]",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                cerr << "Syntax: ambuf <buffer name>" << endl;

                return {};
            }

            buffer_lock<> b(m_buffer);

            if (b.valid())
            {
                buffer_reference buffer(this, nullptr, false);
                buffer.set(args.at(0));
                buffer_lock bl(buffer);
                if (bl.valid())
                {
                    auto mod_s = bl.frame_count();
                    auto mod_c = bl.channel_count();

                    for (auto ch = 0; ch < b.channel_count(); ch++)
                    {
                        for (auto s = 0; s < b.frame_count(); s++)
                        {
                            b.lookup(s, ch) *= (bl.lookup(s % mod_s, ch % mod_c) + 1.0) * 0.5;
                        }
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

    message<> sort
    {
        this,
        "sort",
        "Sort samples in order: sort [a(scending) / d(escending)]",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            bool ascending = true;

            if (args.size() > 0)
            {
                if (std::string(args.at(0)) == "d" || std::string(args.at(0)) == "descending")
                {
                    ascending = false;
                }
            }

            if (b.valid())
            {
                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    std::vector<float> curr_channel(b.frame_count());
                    
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        curr_channel.at(s) = b.lookup(s, ch);
                    }
                    
                    if (ascending)
                    {
                        std::sort(curr_channel.begin(), curr_channel.end());
                    }
                    else
                    {
                        std::sort(curr_channel.begin(), curr_channel.end(), std::greater<float>());
                    }

                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = curr_channel.at(s);
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

    message<> length
    {
        this,
        "length",
        "Change buffer length (changes pitch, too): length [amount] [ms / % / samps]",
        MIN_FUNCTION
        {
            buffer_lock<false> b(m_buffer);

            if (args.size() < 1)
            {
                cerr << "Syntax: length <amount> <unit: ms, % or samps, default ms>" << endl;

                return {};
            }

            std::vector<std::vector<double>> current_buffer;

            if (b.valid())
            {
                double amount = double(args.at(0));

                if (amount <= 0.0)
                {
                    cerr << "Amount must be greater than 0.0" << endl;
                    
                    return {};            
                }

                if (args.size() > 1)
                {
                    if (args.at(1) == "samps")
                    {
                        amount *= 1000.0;
                        amount /= b.samplerate();
                    }
                    if (args.at(1) == "%")
                    {
                        amount *= b.length_in_seconds() * 10.0;
                    }
                }

                amount = std::ceil(amount);

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {    
                    std::vector<double> current_channel(b.frame_count());

                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        current_channel.at(s) = (double)b.lookup(s, ch);
                    }

                    current_buffer.push_back(current_channel);
                }

                b.resize(amount);
                
                b.dirty();
            }
                
            buffer_lock<> b_new(m_buffer);

            if (b_new.valid())
            {
                for (auto ch = 0; ch < b_new.channel_count(); ch++)
                {
                    std::vector<double> new_channel = resize_chunk(current_buffer.at(ch), (unsigned int)b_new.frame_count());

                    for (auto s = 0; s < b_new.frame_count(); s++)
                    {
                        b_new.lookup(s, ch) = new_channel.at(s);
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

    message<> autoconv
    {
        this,
        "autoconv",
        "[EXPERIMENTAL] Convolve the buffer with itself",
        MIN_FUNCTION
        {
            buffer_lock<false> b(m_buffer);
            
            std::vector<std::vector<double>> new_buffer;

            if (b.valid())
            {
                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    if (b.frame_count() > 50000)
                    {
                        cout << "Currently autoconvolution is experimental and only for short samples" << endl;
                        return {};
                    }

                    std::vector<double> channel(b.frame_count());
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        channel.at(s) = b.lookup(s, ch);
                    }

                    std::vector<double> convolved = convolve(channel, channel);
                    new_buffer.push_back(convolved);
                }
                
                auto new_size = b.frame_count() * 2 - 1;
                b.resize_in_samples(new_size);
                b.dirty();  
            }
            else
            {
                return {};
            }

            buffer_lock<> b_new(m_buffer);

            if (b_new.valid())
            {
                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    std::vector<double> channel;
                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b_new.lookup(s, ch) = new_buffer.at(ch).at(s);
                    }
                }

                b_new.dirty();

            }

            return {};
        }
    };    
    
    message<> glue
    {
        this,
        "glue",
        "Extend the buffer with the content from another buffer: glue [buffer name] [b(efore) / a(fter), default a(fter)]",
        MIN_FUNCTION
        {
            buffer_lock<false> b(m_buffer);
            bool after = true;

            if (args.size() < 1)
            {
                cerr << "Syntax: glue <buffer name> <b / a, default a (place the buffer after the current data)>" << endl;

                return {};
            }

            if (args.size() > 1)
            {
                if (std::string(args.at(1)) == "b" || std::string(args.at(1)) == "before")
                {
                    after = false;
                }
            }

            if (b.valid())
            {                
                buffer_reference buffer(this, nullptr, false);
                buffer.set(args.at(0));
                buffer_lock bl(buffer);
                std::vector<std::vector<double>> new_buffer;
                
                if (bl.valid())
                {
                    auto new_size = bl.frame_count() + b.frame_count();

                    for (auto ch = 0; ch < b.channel_count(); ch++)
                    {
                        std::vector<double> new_channel(new_size, 0.0);

                        for (auto s = 0; s < new_size; s++)
                        {
                            if (after)
                            {
                                if (s < b.frame_count())
                                {
                                    new_channel.at(s) = b.lookup(s, ch);
                                }
                                else
                                {
                                    if (ch < bl.channel_count())
                                    {
                                        new_channel.at(s) = bl.lookup(s - b.frame_count(), ch);
                                    }
                                }
                            }
                            else
                            {   
                                if (s < bl.frame_count() && ch < bl.channel_count())
                                {
                                    new_channel.at(s) = bl.lookup(s, ch);
                                }
                                else
                                {
                                    new_channel.at(s) = b.lookup(s - bl.frame_count(), ch);
                                }
                            }

                        }

                        new_buffer.push_back(new_channel);
                    }

                    b.resize_in_samples(new_size);
                    bl.dirty();
                    b.dirty();
                }

                buffer_lock<> b_new(m_buffer);

                if (b_new.valid())
                {
                    for (auto ch = 0; ch < b_new.channel_count(); ch++)
                    {
                        for (auto s = 0; s < b_new.frame_count(); s++)
                        {
                            b_new.lookup(s, ch) = new_buffer.at(ch).at(s);
                        }
                    }

                    b_new.dirty();
                }
            }        

            return {};
        }
    };
    
    message<> spec
    {
        this,
        "spec",
        "Apply a spectral transformation: spec [type: peak / blur / rsupp] [frame size, default 4096]",
        MIN_FUNCTION
        {
            buffer_lock<> b(m_buffer);

            if (b.valid())
            {                
                enum class transforms : int { none, peak, filter, shift, enum_count };
                auto transform = transforms::none;
                int frame_size = 4096;
                float option = 0.0f;

                if (!args.empty())
                {
                    std::string t = std::string(args.at(0));
                    if (t == "peak")
                    {
                        transform = transforms::peak;
                    }
                    else if (t == "filter")
                    {
                        transform = transforms::filter;
                    }
                    else if (t == "shift")
                    {
                        transform = transforms::shift;
                    }

                    if (args.size() > 1)
                    {
                        option = float(args.at(1));
                    }

                    /*
                    int n = int(args.at(2));
                    if (n >= 32 && n <= 32768 && (n & (n - 1)) == 0)
                    {
                        frame_size = n;
                    }
                        */
                }

                int hop_size = frame_size / 2;
                int padding = frame_size - hop_size;
                window_.resize(frame_size);
                fill_window();

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    std::vector<sample> new_channel(b.frame_count() + padding, 0.0);
                    for (auto f = -padding; f < static_cast<int>(b.frame_count()); f += hop_size)
                    {
                        // Read frame
                        dj::fft_arg<sample> fft_frame(frame_size);
					    std::vector<sample> current_frame(frame_size, 0.0);
    					for (auto s = 0; s < frame_size; ++s)
	    				{
                            auto curr_sample = f + s;
                            if (curr_sample < b.frame_count())
			    			{
                                if (curr_sample < 0)
                                {
                                    fft_frame[s] = std::complex<sample>(0.0, 0.0);
                                }
		    				    else
                                {
                                    fft_frame[s] = std::complex<sample>(b.lookup(curr_sample, ch) * window_[s], 0.0);
                                }
					    	}
						    else
						    {
                                fft_frame[s] = std::complex<sample>(0.0, 0.0);
						    }
					    }

                        auto fft_data = dj::fft1d(fft_frame, dj::fft_dir::DIR_FWD);
                        auto h_size = fft_data.size() / 2 + 1;

                        if (transform == transforms::peak)
                        {
                            auto k = std::clamp(static_cast<int>(option), 1, static_cast<int>(h_size));
                            std::vector<std::pair<float, size_t>> magnitudes;
                            magnitudes.reserve(h_size);
                            for (size_t i = 0; i < h_size; ++i)
                            {
                                float mag = std::abs(fft_data[i]);
                                magnitudes.emplace_back(mag, i);
                            }
                        
                            std::partial_sort(magnitudes.begin(), magnitudes.begin() + k, magnitudes.end(),
                                [](const auto& a, const auto& b) { return a.first > b.first; });

                            std::vector<bool> keep(frame_size, false);
                            for (int i = 0; i < k; ++i)
                            {
                                size_t idx = magnitudes[i].second;
                                keep[idx] = true;
                                // Also keep the symmetric bin
                                if (idx > 0 && idx < frame_size / 2)
                                {
                                    size_t sym_idx = frame_size - idx;
                                keep[sym_idx] = true;
                                }
                            }
                    

                            for (size_t i = 0; i < fft_data.size(); ++i)
                            {
                                if (!keep[i])
                                {
                                    fft_data[i] = {0.0f, 0.0f};
                                }
                            }
                        }
                        else if (transform == transforms::filter)
                        {
                            float cutoff = std::clamp(option, 0.0f, static_cast<float>(b.samplerate()) * 0.5f);
                            size_t cutoff_bin = static_cast<size_t>(cutoff * frame_size / b.samplerate());
                            for (size_t i = 0; i < fft_data.size(); ++i)
                            {
                                if (i > cutoff_bin && i < frame_size - cutoff_bin)
                                {
                                    fft_data[i] = {0.0f, 0.0f};
                                }
                            }
                        }
                        else if (transform == transforms::shift)
                        {
                            float shift_freq = std::clamp(option, -static_cast<float>(b.samplerate()) * 0.5f, static_cast<float>(b.samplerate()) * 0.5f);
                            int shift_bins = static_cast<int>(shift_freq * frame_size / b.samplerate());
                            dj::fft_arg<sample> shifted_fft(frame_size, {0.0f, 0.0f});
                            for (size_t i = 0; i < fft_data.size(); ++i)            
                            {
                                int shifted_index = static_cast<int>(i) + shift_bins;
                                if (shifted_index >= 0 && shifted_index < static_cast<int>(frame_size))
                                {
                                    shifted_fft[shifted_index] = fft_data[i];
                                }
                                // Symmetric bin
                                int sym_index = static_cast<int>(frame_size) - static_cast<int>(i);
                                int shifted_sym_index = sym_index + shift_bins;
                                if (shifted_sym_index >= 0 && shifted_sym_index < static_cast<int>(frame_size))
                                {
                                    shifted_fft[shifted_sym_index] = fft_data[sym_index];
                                }
                            }
                            fft_data = shifted_fft;
                        }

                        auto ifft_data = dj::fft1d(fft_data, dj::fft_dir::DIR_BWD);

    					for (auto s = 0; s < frame_size; ++s)
	    				{
                            auto curr_sample = f + s;
		    				if (curr_sample >= 0 && curr_sample < b.frame_count())
			    			{
                                new_channel[curr_sample] += ifft_data[s].real();// * window_[s];
					    	}

                            // Avoid discontinuities due to windowing
                            {
                                //new_channel[curr_sample] /= window_[s];
                            }
					    }
                    }

                    for (auto s = 0; s < b.frame_count(); s++)
                    {
                        b.lookup(s, ch) = new_channel[s];
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
                std::vector<double> curr_channel(b.frame_count());

                for (int s = 0; s < b.frame_count(); s++)
                {
                    curr_channel.at(s) = b.lookup(s, ch);
                }

                original_buffer_.push_back(curr_channel);
            }
        }
    }

    std::vector<double> convolve(const std::vector<double> &x, const std::vector<double> &y, const bool &clipguard = true)
    {
        int n = x.size();
        int m = y.size();
        int c_size = n + m - 1;

        std::vector<double> c(c_size, 0);

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                c.at(i + j) += x.at(i) * y.at(j);
            }
        }

        if (clipguard)
        {
            double max_value = 0.0;
            for (auto &s : c)
            {
                if (max_value < abs(s))
                {
                    max_value = abs(s);
                }
            }

            if (max_value > 1.0)
            {
                double norm_factor = 1.0 / max_value;
                for (int i = 0; i < c_size; i++)
                {
                    c.at(i) *= norm_factor;
                }
            }
        }

        return c;
    }

    static constexpr sample double_pi_ = 3.14159265358979323846 * 2.0;
    std::vector<sample> window_;
	
	inline void fill_window()
	{
		sample w_mul  = 1.0 / (static_cast<sample>(window_.size()) - 1.0);

		for (auto n = 0; n < window_.size(); n++)
		{
			window_[n] = 0.5 * (1.0 - cos(double_pi_ * n * w_mul));
		}
	}
};

MIN_EXTERNAL(buffx);
