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

#ifndef ALLPASS_H_
#define ALLPASS_H_

#include <iostream>

#include "delay.h"

template <class TSample>
class Allpass
{
public:
    Allpass(const TSample &sample_rate = 44100.0,
            const TSample &max_delay_time = 1000.0,
            const TSample &delay_time = 1000.0,
            const TSample &gain = 0.707);

    bool set_sample_rate(const TSample &sample_rate);
    bool set_time(const TSample &delay_time);
    bool set_max_time(const TSample &max_delay_time, bool clear = true);
    bool set_gain(const TSample &gain);
    void clear();

    TSample get_time();
    int get_samples();
    TSample get_max_time();
    TSample get_gain();

    inline TSample run(const TSample &input);
    inline void run(const TSample &input, TSample &output);

    inline TSample get_last_sample();

private:
    TSample sample_rate_;
    TSample max_delay_time_;
    TSample delay_time_;
    TSample output_;

    TSample gain_;

    Delay<TSample> ff_delay_;
    Delay<TSample> fb_delay_;
};

template <class TSample>
Allpass<TSample>::Allpass(const TSample &sample_rate,
                          const TSample &max_delay_time,
                          const TSample &delay_time,
                          const TSample &gain)
{
    if (sample_rate > 0.0)
    {
        sample_rate_ = sample_rate;
    }
    else
    {
        sample_rate_ = 44100.0;
    }

    ff_delay_.set_sample_rate(sample_rate_);
    fb_delay_.set_sample_rate(sample_rate_);

    if (max_delay_time > 0.0)
    {
        max_delay_time_ = max_delay_time;
    }
    else
    {
        max_delay_time_ = 1000.0;
    }

    ff_delay_.set_max_time(max_delay_time_);
    fb_delay_.set_max_time(max_delay_time_);

    if (delay_time <= max_delay_time_)
    {
        delay_time_ = delay_time;
    }
    else
    {
        delay_time_ = max_delay_time_;
    }

    ff_delay_.set_time(delay_time_);
    fb_delay_.set_time(delay_time_);

    set_gain(gain);
}

template <class TSample>
bool Allpass<TSample>::set_sample_rate(const TSample &sample_rate)
{
    if (sample_rate > 0.0)
    {
        sample_rate_ = sample_rate;

        ff_delay_.set_sample_rate(sample_rate_);
        fb_delay_.set_sample_rate(sample_rate_);

        return true;
    }

    std::cerr << "Allpass::sample_rate_ must be greater than zero\n";

    return false;
}

template <class TSample>
bool Allpass<TSample>::set_max_time(const TSample &max_delay_time, bool clear)
{
    if (max_delay_time > 0.0)
    {
        max_delay_time_ = max_delay_time;

        ff_delay_.set_max_time(max_delay_time_, clear);
        fb_delay_.set_max_time(max_delay_time_, clear);

        if (delay_time_ > max_delay_time_)
        {
            delay_time_ = max_delay_time_;
        }

        return true;
    }

    std::cerr << "Allpass::max_delay_time_ must be greater than zero\n";

    return false;
}

template <class TSample>
bool Allpass<TSample>::set_time(const TSample &delay_time)
{
    if (delay_time >= 0.0 && delay_time <= max_delay_time_)
    {
        delay_time_ = delay_time;

        ff_delay_.set_time(delay_time_);
        fb_delay_.set_time(delay_time_);

        return true;
    }

    std::cerr << "Allpass::delay_time_ must be inside [0.0, Allpass::max_delay_time_] range\n";

    return false;
}

template <class TSample>
bool Allpass<TSample>::set_gain(const TSample &gain)
{
    gain_ = gain;

    return true;
}

template <class TSample>
TSample Allpass<TSample>::get_time()
{
    return delay_time_;
}

template <class TSample>
TSample Allpass<TSample>::get_max_time()
{
    return max_delay_time_;
}

template <class TSample>
TSample Allpass<TSample>::get_gain()
{
    return gain_;
}

template <class TSample>
void Allpass<TSample>::clear()
{
    ff_delay_.clear();
    fb_delay_.clear();
}

template <class TSample>
inline TSample Allpass<TSample>::run(const TSample &input)
{
    TSample out = -1.0 * gain_ * input;
    out += ff_delay_.run(input);
    out += gain_ * fb_delay_.run(output_);
    output_ = out;

    return output_;
}

template <class TSample>
inline void Allpass<TSample>::run(const TSample &input, TSample &output)
{
    output_ = run(input);
    output = output_;
}

template <class TSample>
inline TSample Allpass<TSample>::get_last_sample()
{
    return output_;
}

#endif // ALLPASS_H_
