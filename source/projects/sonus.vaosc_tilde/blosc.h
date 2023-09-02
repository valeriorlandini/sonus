/******************************************************************************
Copyright (c) 2020-2023 Sonus Dept.

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


#ifndef BLOSC_H_
#define BLOSC_H_

#include <algorithm>
#include <cmath>

namespace amtl
{
enum BLWaveforms
{
    SINE,
    TRIANGLE,
    SAW,
    SQUARE
};

template <class TSample>
class BLOsc
{
public:
    BLOsc(const TSample &sample_rate = 44100.0,
          const TSample &frequency = 0.0);

    bool set_sample_rate(const TSample &sample_rate);
    bool set_frequency(const TSample &frequency);
    void reset();

    inline bool run();
    inline bool run(TSample &sine_out, TSample &triangle_out, TSample &saw_out,
                    TSample &square_out);

    inline void get_last_sample(TSample &sine_out, TSample &triangle_out,
                                TSample &saw_out, TSample &square_out);
    inline TSample get_sine()
    {
        return sine_out_;
    };
    inline TSample get_triangle()
    {
        return triangle_out_;
    };
    inline TSample get_saw()
    {
        return saw_out_;
    };
    inline TSample get_square()
    {
        return square_out_;
    };

private:
    TSample frequency_;
    TSample inv_sample_rate_;
    TSample half_sample_rate_;

    TSample step_;
    TSample ramp_;

    TSample harmonics_;

    TSample saw_out_;
    TSample sine_out_;
    TSample triangle_out_;
    TSample square_out_;
};

template <class TSample>
BLOsc<TSample>::BLOsc(const TSample &sample_rate, const TSample &frequency)
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

    reset();
}

template <class TSample>
bool BLOsc<TSample>::set_sample_rate(const TSample &sample_rate)
{
    if (sample_rate > 0.0)
    {
        half_sample_rate_ = sample_rate * 0.5;
        inv_sample_rate_ = 1.0 / sample_rate;

        set_frequency(frequency_);

        return true;
    }

    std::cerr << "BLOsc::sample_rate must be greater than zero\n";

    return false;
}

template <class TSample>
bool BLOsc<TSample>::set_frequency(const TSample &frequency)
{
    if (frequency < half_sample_rate_)
    {
        frequency_ = frequency;
    }
    else
    {
        frequency_ = half_sample_rate_ * 0.999;
    }

    step_ = frequency_ * inv_sample_rate_;

    if (frequency != 0.0)
    {
        harmonics_ = std::min(30.0, floor(half_sample_rate_ / abs(frequency_)));
    }
    else
    {
        harmonics_ = 0.0;
    }

    return true;
}

template <class TSample>
void BLOsc<TSample>::reset()
{
    ramp_ = 0.0;
}

template <class TSample>
inline bool BLOsc<TSample>::run()
{
    bool new_cycle = false;

    ramp_ += step_;
    if (ramp_ > 1.0)
    {
        while (ramp_ > 1.0)
        {
            ramp_ -=  1.0;
        }

        new_cycle = true;
    }

    sine_out_ = sin(ramp_ * M_PI * 2.0);

    saw_out_ = 0.0;
    square_out_ = 0.0;
    triangle_out_ = 0.0;

    for (TSample harmonic = 1.0; harmonic <= harmonics_; harmonic++)
    {
        saw_out_ += sin(-ramp_ * M_PI * 2.0 * harmonic) / harmonic;

        if ((unsigned int)harmonic % 2)
        {
            square_out_ += sin(ramp_ * M_PI * 2.0 * harmonic) / harmonic;
            triangle_out_ += cos(ramp_ * M_PI * 2.0 * harmonic) / (harmonic * harmonic);
        }
    }

    saw_out_ *= 0.55;
    square_out_ *= 1.07;
    triangle_out_ *= 0.82;

    return new_cycle;
}

template <class TSample>
inline bool BLOsc<TSample>::run(TSample &sine_out, TSample &triangle_out,
                                TSample &saw_out, TSample &square_out)
{
    bool new_cycle = run();

    get_last_sample(sine_out, triangle_out, saw_out, square_out);

    return new_cycle;
}

template <class TSample>
inline void BLOsc<TSample>::get_last_sample(TSample &sine_out,
                                            TSample &triangle_out,
                                            TSample &saw_out,
                                            TSample &square_out)
{
    saw_out = saw_out_;
    sine_out = sine_out_;
    triangle_out = triangle_out_;
    square_out = square_out_;
}
}

#endif // BLOSC_H_