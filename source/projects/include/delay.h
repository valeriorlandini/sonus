/******************************************************************************
Copyright (c) 2023 Valerio Orlandini

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef DELAY_H_
#define DELAY_H_

#include <algorithm>
#include <cmath>
#include <vector>
#include <iostream>

#include "interp.h"

template <class TSample>
class Delay
{
public:
    Delay(const TSample &sample_rate = 44100.0,
          const TSample &max_delay_time = 5000.0);

    bool set_sample_rate(const TSample &sample_rate);
    bool set_time(const TSample &delay_time);
    bool set_max_time(const TSample &max_delay_time, bool clear = true);
    //bool set_delay_sample(const TSample &);
    bool set_feedback(const TSample &feedback);
    void clear();

    TSample get_time();
    int get_samples();
    TSample get_max_time();
    TSample get_feedback();

    inline TSample run(const TSample &input);
    inline void run(const TSample &input, TSample &output);

    inline TSample get_last_sample();

private:
    TSample max_delay_time_;
    TSample sample_rate_;
    TSample delay_time_;

    int delay_samples_[2];
    TSample delay_interp_;

    TSample feedback_;

    TSample output_;

    int read_pos_[2];
    int write_pos_;

    std::vector<TSample> buffer_;
};

template <class TSample>
Delay<TSample>::Delay(const TSample &sample_rate, const TSample &max_delay_time)
{
    if (sample_rate > 0.0)
    {
        sample_rate_ = sample_rate;
    }
    else
    {
        sample_rate_ = 44100.0;
    }

    if (max_delay_time > 0.0)
    {
        max_delay_time_ = max_delay_time;
    }
    else
    {
        max_delay_time_ = 5000.0;
    }

    buffer_.assign((unsigned int)ceil(max_delay_time_ * sample_rate_ * 0.001) + 1, 0.0);

    write_pos_ = 0;

    set_time(max_delay_time_);
}

template <class TSample>
bool Delay<TSample>::set_sample_rate(const TSample &sample_rate)
{
    if (sample_rate > 0.0)
    {
        sample_rate_ = sample_rate;

        set_max_time(max_delay_time_);
        set_time(delay_time_);

        return true;
    }

    std::cerr << "Delay::sample_rate_ must be greater than zero\n";

    return false;
}

template <class TSample>
bool Delay<TSample>::set_max_time(const TSample &max_delay_time, bool clear)
{
    if (max_delay_time > 0.0)
    {
        max_delay_time_ = max_delay_time;

        if (clear)
        {
            buffer_.assign((unsigned int)ceil(max_delay_time_ * sample_rate_ * 0.001) + 1, 0.0);
        }
        else
        {
            buffer_.resize((unsigned int)ceil(max_delay_time_ * sample_rate_ * 0.001) + 1, 0.0);
        }

        if (delay_time_ > max_delay_time_)
        {
            set_delay_time(max_delay_time_);
        }

        return true;
    }

    std::cerr << "Delay::max_delay_time_ must be greater than zero\n";

    return false;
}

template <class TSample>
bool Delay<TSample>::set_time(const TSample &delay_time)
{
    if (delay_time >= 0.0 && delay_time <= max_delay_time_)
    {
        delay_time_ = delay_time;

        delay_samples_[0] = (int)floor(sample_rate_ * delay_time_ * 0.001);
        delay_samples_[1] = (int)ceil(sample_rate_ * delay_time_ * 0.001) % buffer_.size();
        delay_interp_ = (sample_rate_ * delay_time_ * 0.001) - floor(sample_rate_ * delay_time_ * 0.001);

        for (int p = 0; p <= 1; p++)
        {
            read_pos_[p] = write_pos_ - delay_samples_[p];
            if (read_pos_[p] < 0)
            {
                read_pos_[p] = buffer_.size() + read_pos_[p];
            }
        }

        return true;
    }

    std::cerr << "Delay::delay_time_ must be inside [0.0, Delay::max_delay_time_] range\n";

    return false;
}

template <class TSample>
bool Delay<TSample>::set_feedback(const TSample &feedback)
{
    feedback_ = feedback;

    return true;
}

template <class TSample>
TSample Delay<TSample>::get_time()
{
    return delay_time_;
}

template <class TSample>
int Delay<TSample>::get_samples()
{
    return delay_samples_;
}

template <class TSample>
TSample Delay<TSample>::get_max_time()
{
    return max_delay_time_;
}

template <class TSample>
TSample Delay<TSample>::get_feedback()
{
    return feedback_;
}

template <class TSample>
void Delay<TSample>::clear()
{
    buffer_.assign(buffer_.size(), 0.0);
}

template <class TSample>
inline TSample Delay<TSample>::run(const TSample &input)
{
    output_ = cosip(buffer_.at(read_pos_[0]), buffer_.at(read_pos_[1]), delay_interp_);

    buffer_.at(write_pos_) = input + (output_ * feedback_);

    if (++write_pos_ >= buffer_.size())
    {
        write_pos_ = 0;
    }

    for (int p = 0; p <= 1; p++)
    {
        if (++read_pos_[p] >= buffer_.size())
        {
            read_pos_[p] = 0;
        }
    }

    return output_;
}

template <class TSample>
inline void Delay<TSample>::run(const TSample &input, TSample &output)
{
    output_ = run(input);
    output = output_;
}

template <class TSample>
inline TSample Delay<TSample>::get_last_sample()
{
    return output_;
}

#endif // DELAY_H_
