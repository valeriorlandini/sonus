/// @file       sonus.wavesets.cpp
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <random>
#include "biquad.h"
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
                    int groups = std::max(1, int(ceil((double)wavesets_idx_.at(ch).size() / (double)group_size)));
                    if (groups == 1)
                    {
                        continue;
                    }

                    auto rand_dev = std::random_device{}; 
                    auto rand_eng = std::default_random_engine{rand_dev()};
                    std::vector<int> ws(groups);
                    std::iota(std::begin(ws), std::end(ws), 0);
                    std::shuffle(std::begin(ws), std::end(ws), rand_eng);
                    std::vector<double> new_wave;

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
                        
                        std::vector<double> curr_waveset;

                        for (int s = end; s >= start; --s)
                        {
                            curr_waveset.push_back((double)b.lookup(s, ch));
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
                        
                        std::vector<double> curr_waveset;

                        for (int s = end; s >= start; --s)
                        {
                            curr_waveset.push_back((double)b.lookup(s, ch));
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
            buffer_lock<false> b(m_buffer);

            int avg_group = 1;

            if (!args.empty())
            {
                avg_group = std::max(1, int(args.at(0)));
            }


            if (b.valid())
            {
                std::vector<std::vector<double>> new_buffer;
                int max_length = 0;

                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    int w = 0;
                    std::vector<double> new_channel;
                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += avg_group)
                    {
                        int start_curr = wavesets_idx_.at(ch).at(w).start;
                        int end_curr = wavesets_idx_.at(ch).at((w + avg_group - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        int start_next = wavesets_idx_.at(ch).at((w + avg_group) % wavesets_idx_.at(ch).size()).start;
                        int end_next = wavesets_idx_.at(ch).at((w + (avg_group * 2) - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        std::vector<double> curr_waveset;
                        std::vector<double> next_waveset;

                        if (end_curr < start_curr)
                        {
                            end_curr += b.frame_count();
                        }
                        for (int s = start_curr; s <= end_curr; ++s)
                        {
                            curr_waveset.push_back((double)b.lookup(s % b.frame_count(), ch));
                        }

                        if (end_next < start_next)
                        {
                            end_next += b.frame_count();
                        }
                        for (int s = start_next; s <= end_next; ++s)
                        {
                            next_waveset.push_back((double)b.lookup(s % b.frame_count(), ch));
                        }

                        auto avg_size = (curr_waveset.size() + next_waveset.size()) / 2;
                        std::vector<double> resized_curr = resize_chunk(curr_waveset, avg_size);
                        std::vector<double> resized_next = resize_chunk(next_waveset, avg_size);

                        for (int s = 0; s < avg_size; s++)
                        {
                            new_channel.push_back((resized_curr.at(s) + resized_next.at(s)) * 0.5);
                        }
                    }

                    new_buffer.push_back(new_channel);

                    if (new_channel.size() > max_length)
                    {
                        max_length = new_channel.size();
                    }
                }
                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    if (new_buffer.at(ch).size() < max_length)
                    {
                        new_buffer.at(ch).resize(max_length, 0.0);
                    }
                }

                b.resize_in_samples(max_length);

                b.dirty();

                buffer_lock<> b_new(m_buffer);

                if (b_new.valid())
                {
                    for (auto ch = 0; ch < b_new.channel_count(); ch++)
                    {
                        for (auto s = 0; s < b_new.frame_count(); s++)
                        {
                            if (new_buffer.at(ch).size() > s)
                            {
                                b_new.lookup(s, ch) = new_buffer.at(ch).at(s);
                            }
                        }
                    }

                    b_new.dirty();
                }
            }

            return {};
        }
    };

    message<> mirshrink
    {
        this,
        "mirshrink",
        "Mirror-copy a group of wavesets and then shrink the group so that it keeps the original size",
        MIN_FUNCTION
        {
            analyse();
            buffer_lock<> b(m_buffer);

            int mir_group = 1;

            if (!args.empty())
            {
                mir_group = std::max(1, int(args.at(0)));
            }


            if (b.valid())
            {
                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    int w = 0;
                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += mir_group)
                    {
                        int start = wavesets_idx_.at(ch).at(w).start;
                        int end = wavesets_idx_.at(ch).at(std::min(int(wavesets_idx_.at(ch).size()) - 1, w + mir_group - 1)).end;
                        
                        std::vector<double> curr_waveset;

                        for (int s = start; s <= end; ++s)
                        {
                            curr_waveset.push_back((double)b.lookup(s, ch));
                        }
                        for (int s = end; s >= start; --s)
                        {
                            curr_waveset.push_back((double)b.lookup(s, ch));
                        }

                        auto mirshrink_ws = resize_chunk(curr_waveset, curr_waveset.size() / 2);
     
                        int pos = 0;

                        for (int s = start; s <= end; ++s)
                        {
                            b.lookup(s, ch) = mirshrink_ws.at(pos++);
                        }
                    }

                    if (w - mir_group < wavesets_idx_.at(ch).size() - 1)
                    {
                        int start = wavesets_idx_.at(ch).at(w - mir_group).start;
                        int end = wavesets_idx_.at(ch).at(int(wavesets_idx_.at(ch).size()) - 1).end;
                        
                        std::vector<double> curr_waveset;

                        for (int s = start; s <= end; ++s)
                        {
                            curr_waveset.push_back((double)b.lookup(s, ch));
                        }
                        for (int s = end; s >= start; --s)
                        {
                            curr_waveset.push_back((double)b.lookup(s, ch));
                        }
     
                        auto mirshrink_ws = resize_chunk(curr_waveset, curr_waveset.size() / 2);

                        int pos = 0;

                        for (int s = start; s <= end; ++s)
                        {
                            b.lookup(s, ch) = mirshrink_ws.at(pos++);
                        }
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
        "Multiply each group of wavesets with the next one, resizing the second accordingly",
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
                    std::vector<double> new_buffer;
                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += avg_group)
                    {
                        int start_curr = wavesets_idx_.at(ch).at(w).start;
                        int end_curr = wavesets_idx_.at(ch).at((w + avg_group - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        int start_next = wavesets_idx_.at(ch).at((w + avg_group) % wavesets_idx_.at(ch).size()).start;
                        int end_next = wavesets_idx_.at(ch).at((w + (avg_group * 2) - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        std::vector<double> curr_waveset;
                        std::vector<double> next_waveset;

                        if (end_curr < start_curr)
                        {
                            end_curr += b.frame_count();
                        }
                        for (int s = start_curr; s <= end_curr; ++s)
                        {
                            curr_waveset.push_back((double)b.lookup(s % b.frame_count(), ch));
                        }

                        if (end_next < start_next)
                        {
                            end_next += b.frame_count();
                        }
                        for (int s = start_next; s <= end_next; ++s)
                        {
                            next_waveset.push_back((double)b.lookup(s % b.frame_count(), ch));
                        }
     
                        std::vector<double> resized_next = resize_chunk(next_waveset, curr_waveset.size());

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

    message<> mix
    {
        this,
        "mix",
        "Mix each group of wavesets with the next one, resizing the second accordingly",
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
                    std::vector<double> new_buffer;
                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += avg_group)
                    {
                        int start_curr = wavesets_idx_.at(ch).at(w).start;
                        int end_curr = wavesets_idx_.at(ch).at((w + avg_group - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        int start_next = wavesets_idx_.at(ch).at((w + avg_group) % wavesets_idx_.at(ch).size()).start;
                        int end_next = wavesets_idx_.at(ch).at((w + (avg_group * 2) - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        std::vector<double> curr_waveset;
                        std::vector<double> next_waveset;

                        if (end_curr < start_curr)
                        {
                            end_curr += b.frame_count();
                        }
                        for (int s = start_curr; s <= end_curr; ++s)
                        {
                            curr_waveset.push_back((double)b.lookup(s % b.frame_count(), ch));
                        }

                        if (end_next < start_next)
                        {
                            end_next += b.frame_count();
                        }
                        for (int s = start_next; s <= end_next; ++s)
                        {
                            next_waveset.push_back((double)b.lookup(s % b.frame_count(), ch));
                        }
     
                        std::vector<double> resized_next = resize_chunk(next_waveset, curr_waveset.size());

                        for (int s = 0; s < curr_waveset.size(); s++)
                        {
                            new_buffer.push_back((curr_waveset.at(s) + resized_next.at(s)) * 0.707f);
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
                    std::vector<double> new_buffer;
                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += avg_group)
                    {
                        int start_curr = wavesets_idx_.at(ch).at(w).start;
                        int end_curr = wavesets_idx_.at(ch).at((w + avg_group - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        int start_next = wavesets_idx_.at(ch).at((w + avg_group) % wavesets_idx_.at(ch).size()).start;
                        int end_next = wavesets_idx_.at(ch).at((w + (avg_group * 2) - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        std::vector<double> curr_waveset;
                        std::vector<double> next_waveset;

                        if (end_curr < start_curr)
                        {
                            end_curr += b.frame_count();
                        }
                        for (int s = start_curr; s <= end_curr; ++s)
                        {
                            curr_waveset.push_back((double)b.lookup(s % b.frame_count(), ch));
                        }

                        if (end_next < start_next)
                        {
                            end_next += b.frame_count();
                        }
                        for (int s = start_next; s <= end_next; ++s)
                        {
                            next_waveset.push_back((double)b.lookup(s % b.frame_count(), ch));
                        }
     
                        std::vector<double> resized_next = resize_chunk(next_waveset, curr_waveset.size());

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

    message<> stretch
    {
        this,
        "stretch",
        "Adjust the length of each group of n wavesets according to m stretch factor, the sequence can be of arbitrary length (i.e. n1 m1 n2 m2 n3 m3...)",
        MIN_FUNCTION
        {
            analyse();
            buffer_lock<false> b(m_buffer);

            if (args.empty() || (args.size() % 2))
            {
                cout << "Syntax: stretch n1 m1 n2 m2 n3 m3 ..." << endl;
                return {};
            }

            if (b.valid())
            {
                struct block
                {
                    int length;
                    double stretch;
                };

                std::vector<block> seq;

                for (auto a = 0; a < args.size(); a+=2)
                {
                    block curr_block;
                    curr_block.length = std::max(0, int(args.at(a)));
                    curr_block.stretch = std::clamp(double(args.at(a+1)), 0.01, 100.0);
                    seq.push_back(curr_block);
                }

                std::vector<std::vector<double>> new_buffer; 
                int max_length = 0;

                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    std::vector<double> new_channel;

                    int w = 0;
                    int seq_pos = 0;

                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += seq.at(seq_pos).length)
                    {
                        int start = wavesets_idx_.at(ch).at(w).start;
                        int end = wavesets_idx_.at(ch).at(std::min(int(wavesets_idx_.at(ch).size()) - 1, w + seq.at(seq_pos).length - 1)).end;
                        
                        std::vector<double> curr_waveset;

                        for (auto s = start; s <= end; s++)
                        {
                            curr_waveset.push_back((double)b.lookup(s, ch));
                        }

                        auto res_waveset = resize_chunk(curr_waveset, (unsigned int)(std::ceil((double)curr_waveset.size() * seq.at(seq_pos).stretch)));
                        
                        new_channel.insert(new_channel.end(), res_waveset.begin(), res_waveset.end());

                        ++seq_pos;
                        seq_pos %= seq.size();
                    }

                    new_buffer.push_back(new_channel);

                    if (new_channel.size() > max_length)
                    {
                        max_length = new_channel.size();
                    }
                }

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    if (new_buffer.at(ch).size() < max_length)
                    {
                        new_buffer.at(ch).resize(max_length, 0.0);
                    }
                }

                b.resize_in_samples(max_length);

                b.dirty();

                buffer_lock<> b_new(m_buffer);

                if (b_new.valid())
                {
                    for (auto ch = 0; ch < b_new.channel_count(); ch++)
                    {
                        for (auto s = 0; s < b_new.frame_count(); s++)
                        {
                            if (new_buffer.at(ch).size() > s)
                            {
                                b_new.lookup(s, ch) = new_buffer.at(ch).at(s);
                            }
                        }
                    }

                    b_new.dirty();
                }
            }

            return {};
        }
    };

    message<> filter
    {
        this,
        "filter",
        "Apply to n wavesets a filter (lp, hp, bp, or no for no filter) with a m cutoff, the sequence can be of arbitrary length (e.g. n1 lp m1 n2 no m2...)",
        MIN_FUNCTION
        {
            analyse();
            buffer_lock<> b(m_buffer);

            if (args.empty() || (args.size() % 3))
            {
                cout << "Syntax: filter n1 <filter type (lp, hp, bp, no)> <cutoff> n2 <filter type (lp, hp, bp, no)> <cutoff> ..." << endl;
                return {};
            }

            if (b.valid())
            {
                struct block
                {
                    int length;
                    Biquad<double> filter;
                    bool apply = false;
                };

                std::vector<block> seq;

                for (auto a = 0; a < args.size(); a+=3)
                {
                    block curr_block;
                    curr_block.length = std::max(0, int(args.at(a)));
                    curr_block.filter.set_sample_rate(b.samplerate());
                    if (std::string(args.at(a+1)) == "lp")
                    {
                        curr_block.apply = true;
                        curr_block.filter.set_type(BQFilters::lowpass);
                        curr_block.filter.set_cutoff(double(args.at(a+2)));
                    }
                    if (std::string(args.at(a+1)) == "hp")
                    {
                        curr_block.apply = true;
                        curr_block.filter.set_type(BQFilters::hipass);
                        curr_block.filter.set_cutoff(double(args.at(a+2)));
                    }
                    if (std::string(args.at(a+1)) == "bp")
                    {
                        curr_block.apply = true;
                        curr_block.filter.set_type(BQFilters::bandpass);
                        curr_block.filter.set_cutoff(double(args.at(a+2)));
                    }
                    seq.push_back(curr_block);
                }

                std::vector<std::vector<double>> new_buffer;
                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    std::vector<double> new_channel;

                    int w = 0;
                    int seq_pos = 0;

                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += seq.at(seq_pos).length)
                    {
                        if (seq.at(seq_pos).apply)
                        {
                            int start = wavesets_idx_.at(ch).at(w).start;
                            int end = wavesets_idx_.at(ch).at(std::min(int(wavesets_idx_.at(ch).size()) - 1, w + seq.at(seq_pos).length - 1)).end;
                            
                            seq.at(seq_pos).filter.clear();

                            for (auto s = start; s <= end; s++)
                            {
                                b.lookup(s, ch) = seq.at(seq_pos).filter.run(b.lookup(s, ch));
                            }
                        }

                        ++seq_pos;
                        seq_pos %= seq.size();
                    }
                }

                b.dirty();
            }            

            return {};
        }
    };

    message<> same
    {
        this,
        "same",
        "Resize each group of n wavesets to the same length, that can be expressed in a time unit among ms (milliseconds, default) or sm (samples)",
        MIN_FUNCTION
        {
            analyse();
            buffer_lock<false> b(m_buffer);

            if (args.size() < 2)
            {
                cout << "Syntax: same n <length> <unit (ms, sm) - optional, default ms>" << endl;
                return {};
            }

            if (b.valid())
            {
                int avg_group = std::max(1, int(args.at(0)));
                double length = double(args.at(1));
                int sample_length = static_cast<int>(std::ceil(length * (double)b.samplerate() * 0.001f));

                if (args.size() > 2 && std::string(args.at(2)) == "sm")
                {
                    sample_length = static_cast<int>(length);
                }

                if (sample_length < 2)
                {
                    sample_length = 2; 
                }

                std::vector<std::vector<double>> new_buffer;
                int max_length = 0;

                for (int ch = 0; ch < b.channel_count(); ch++)
                {
                    int w = 0;
                    std::vector<double> new_channel;

                    for (w = 0; w < wavesets_idx_.at(ch).size(); w += avg_group)
                    {
                        int start_curr = wavesets_idx_.at(ch).at(w).start;
                        int end_curr = wavesets_idx_.at(ch).at((w + avg_group - 1) % wavesets_idx_.at(ch).size()).end;
                        
                        std::vector<double> curr_waveset;

                        if (end_curr < start_curr)
                        {
                            end_curr += b.frame_count();
                        }
                        for (int s = start_curr; s <= end_curr; ++s)
                        {
                            curr_waveset.push_back((double)b.lookup(s % b.frame_count(), ch));
                        }
     
                        std::vector<double> resized = resize_chunk(curr_waveset, sample_length);

                        for (int s = 0; s < resized.size(); s++)
                        {
                            new_channel.push_back(resized.at(s));
                        }
                    }

                    new_buffer.push_back(new_channel);

                    if (new_channel.size() > max_length)
                    {
                        max_length = new_channel.size();
                    }
                }

                for (auto ch = 0; ch < b.channel_count(); ch++)
                {
                    if (new_buffer.at(ch).size() < max_length)
                    {
                        new_buffer.at(ch).resize(max_length, 0.0);
                    }
                }

                b.resize_in_samples(max_length);

                b.dirty();

                buffer_lock<> b_new(m_buffer);

                if (b_new.valid())
                {
                    for (auto ch = 0; ch < b_new.channel_count(); ch++)
                    {
                        for (auto s = 0; s < b_new.frame_count(); s++)
                        {
                            if (new_buffer.at(ch).size() > s)
                            {
                                b_new.lookup(s, ch) = new_buffer.at(ch).at(s);
                            }
                        }
                    }

                    b_new.dirty();
                }
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
                            double wave_length = wavesets_idx_.at(ch).at(w).end - wavesets_idx_.at(ch).at(w).start;

                            if (wave_length > 0.0)
                            {
                                for (int s = wavesets_idx_.at(ch).at(w).start; s <= wavesets_idx_.at(ch).at(w).end; s++)
                                {
                                    double ramp = -1.0 * wrap(((wavesets_idx_.at(ch).at(w).end - s) / wave_length) * 2.0, -1.0, 1.0);
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
    std::vector<std::vector<waveset_params<double>>> wavesets_idx_;
    std::vector<std::vector<double>> original_buffer_;

    void analyse()
    {
        buffer_lock<> b(m_buffer);

        if (b.valid())
        {
            wavesets_idx_.clear();

            for (int ch = 0; ch < b.channel_count(); ch++)
            {
                std::vector<waveset_params<double>> curr_channel;
                waveset_params<double> curr_waveset;
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
                        curr_waveset.peak = (double)fabs(b.lookup(s, ch));
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

MIN_EXTERNAL(wavesets);
