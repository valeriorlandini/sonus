/******************************************************************************
Copyright (c) 2020-2021 Sonus Dept.

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

#ifndef VAOSC_H_
#define VAOSC_H_

#include <algorithm>
#include <cmath>
#include <iostream>

// clamp function if the compiler does not support C++17
#if __cplusplus < 201703L
template<class T>
inline T clamp(const T& value, const T& low, const T& high)
{
    return (value < low) ? low : (high < value) ? high : value;
}
#endif

namespace amtl
{
enum VAWaveforms
{
    SINE,
    TRIANGLE,
    SAW,
    PULSE
};

template <class TSample>
class VAOsc
{
public:
    VAOsc(const TSample &sample_rate = 44100.0,
          const TSample &frequency = 0.0);

    bool set_sample_rate(const TSample &sample_rate);
    bool set_frequency(const TSample &frequency);
    bool set_pulse_width(const TSample &pulse_width);
    void reset();

    inline bool run();
    inline bool run(TSample &sine_out, TSample &triangle_out, TSample &saw_out,
                    TSample &pulse_out);
    inline TSample run(const VAWaveforms &wave_1,
                       const VAWaveforms &wave_2 = -1,
                       const TSample &mix = 0.0);

    inline void get_last_sample(TSample &sine_out, TSample &triangle_out,
                                TSample &saw_out, TSample &pulse_out);
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
    inline TSample get_pulse()
    {
        return pulse_out_;
    };

private:
    TSample frequency_;
    TSample inv_sample_rate_;
    TSample half_sample_rate_;

    TSample step_;
    TSample saw_out_;
    TSample sine_out_;
    TSample triangle_out_;
    TSample pulse_out_;
    TSample pulse_width_;
};

template <class TSample>
VAOsc<TSample>::VAOsc(const TSample &sample_rate, const TSample &frequency)
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

    set_pulse_width(0.5);

    reset();
}

template <class TSample>
bool VAOsc<TSample>::set_sample_rate(const TSample &sample_rate)
{
    if (sample_rate > 0.0)
    {
        half_sample_rate_ = sample_rate * 0.5;
        inv_sample_rate_ = 1.0 / sample_rate;

        set_frequency(frequency_);

        return true;
    }

    std::cerr << "VAOsc::sample_rate must be greater than zero\n";

    return false;
}

template <class TSample>
bool VAOsc<TSample>::set_frequency(const TSample &frequency)
{
    frequency_ = frequency;

    step_ = 2.0 * (frequency_ * inv_sample_rate_);

    return true;
}

template <class TSample>
inline bool VAOsc<TSample>::set_pulse_width(const TSample &pulse_width)
{
    if (pulse_width >= 0.0 && pulse_width <= 1.0)
    {
        pulse_width_ = pulse_width;

        return true;
    }

    std::cerr << "VAOsc::pulse_width_ must be inside [0.0, 1.0] range\n";

    return false;
}

template <class TSample>
void VAOsc<TSample>::reset()
{
    sine_out_ = 0.0;
    saw_out_ = 0.0;
    triangle_out_ = 0.0;
    pulse_out_ = 0.0;
}

template <class TSample>
inline bool VAOsc<TSample>::run()
{
    bool new_cycle = false;

    saw_out_ += step_;
    if (fabs(saw_out_) > 1.0)
    {
        saw_out_ = fmod(saw_out_ + 1.0, 2.0) - copysign(1.0, saw_out_);
        new_cycle = true;
    }

    sine_out_ = sin(saw_out_ * M_PI);

    triangle_out_ = (copysign(2.0, -saw_out_) * saw_out_) + 1.0;

    if ((saw_out_ + 1.0) * 0.5 > pulse_width_)
    {
        pulse_out_ = 1.0;
    }
    else
    {
        pulse_out_ = -1.0;
    }

    return new_cycle;
}

template <class TSample>
inline bool VAOsc<TSample>::run(TSample &sine_out, TSample &triangle_out,
                                TSample &saw_out, TSample &pulse_out)
{
    bool new_cycle = run();

    get_last_sample(sine_out, triangle_out, saw_out, pulse_out);

    return new_cycle;
}

template <class TSample>
inline void VAOsc<TSample>::get_last_sample(TSample &sine_out,
                                            TSample &triangle_out,
                                            TSample &saw_out,
                                            TSample &pulse_out)
{
    saw_out = saw_out_;
    sine_out = sine_out_;
    triangle_out = triangle_out_;
    pulse_out = pulse_out_;
}

template <class TSample>
inline TSample VAOsc<TSample>::run(const VAWaveforms &wave_1,
                                   const VAWaveforms &wave_2,
                                   const TSample &mix)
{
    run();

    TSample output = 0.0;
    TSample wave1_mix = sqrt(1.0 - clamp(mix, 0.0, 1.0));
    TSample wave2_mix = sqrt(clamp(mix, 0.0, 1.0));

    switch (wave_1)
    {
    case SINE:
        output += sine_out_ * wave1_mix;
        break;
    case TRIANGLE:
        output += triangle_out_ * wave1_mix;
        break;
    case SAW:
        output += saw_out_ * wave1_mix;
        break;
    case PULSE:
        output += pulse_out_ * wave1_mix;
        break;
    }

    switch (wave_2)
    {
    case SINE:
        output += sine_out_ * wave2_mix;
        break;
    case TRIANGLE:
        output += triangle_out_ * wave2_mix;
        break;
    case SAW:
        output += saw_out_ * wave2_mix;
        break;
    case PULSE:
        output += pulse_out_ * wave2_mix;
        break;
    }

    return output;
}
}

#endif // VAOSC_H_
