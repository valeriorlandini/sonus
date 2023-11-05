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

#ifndef RANDSIG_H_
#define RANDSIG_H_

#include <cmath>
#include <iostream>
#include <random>

template <class TSample>
class Randsig
{
public:
    Randsig(const TSample &sample_rate = 44100.0,
            const TSample &frequency = 1.0);

    bool set_sample_rate(const TSample &sample_rate);
    bool set_frequency(const TSample &frequency);

    void reset();

    inline TSample run();

private:
    TSample sample_rate_;
    TSample half_sample_rate_;

    TSample frequency_;

    TSample sample_count_;
    TSample steps_;

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<TSample> rand_dist_;

    TSample current_;
    TSample next_;
    TSample output_;
};

template <class TSample>
Randsig<TSample>::Randsig(const TSample &sample_rate, const TSample &frequency)
{
    frequency_ = frequency;

    if (sample_rate > 0.0)
    {
        set_sample_rate(sample_rate);
    }
    else
    {
        set_sample_rate(44100.0);
    }

    gen_.seed(rd_());

    reset();
}

template <class TSample>
bool Randsig<TSample>::set_sample_rate(const TSample &sample_rate)
{
    if (sample_rate > 0.0)
    {
        sample_rate_ = sample_rate;
        half_sample_rate_ = sample_rate_ * 0.5;

        set_frequency(frequency_);

        return true;
    }

    std::cerr << "Randsig::sample_rate must be greater than zero\n";

    return false;
}

template <class TSample>
bool Randsig<TSample>::set_frequency(const TSample &frequency)
{
    if (std::abs(frequency) > 0.0 && std::abs(frequency) <= half_sample_rate_)
    {
        frequency_ = frequency;

        steps_ = ceil(sample_rate_ / frequency_);
        sample_count_ = 0.0;

        return true;
    }

    std::cerr << "Randsig::frequency must be inside (0.0, " << half_sample_rate_ << "]\n";

    return false;

}

template <class TSample>
void Randsig<TSample>::reset()
{
    output_ = 0.0;
    current_ = 0.0;
    next_ = (rand_dist_(gen_) * 2.0) - 1.0;
}

template <class TSample>
inline TSample Randsig<TSample>::run()
{
    ++sample_count_;

    if (sample_count_ > steps_)
    {
        current_ = next_;
        next_ = (rand_dist_(gen_) * 2.0) - 1.0;
        sample_count_ = 0.0;
    }

    TSample ratio = sample_count_ / steps_;

    output_ = current_ * (1.0 - ratio) + next_ * ratio;

    return output_;
}

#endif // RANDSIG_H_
