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

#ifndef PULSAR_H_
#define PULSAR_H_

#include <algorithm>
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>

#include "window_functions.h"


enum PulsarWaveforms
{
    SINE,
    TRIANGLE,
    SAW,
    SQUARE,
    NOISE,
    DC,
    PHASOR
};

enum PulsarWindows
{
    HANN,
    HAMMING,
    BLACKMAN,
    NUTTALL,
    BLACKMANNUTTALL,
    BLACKMANHARRIS,
    FLATTOP,
    BARTLETTHANN,
    RECTANGULAR
};

template <class TSample>
class Pulsar
{
public:
    Pulsar(const TSample &sample_rate = 44100.0,
          const TSample &frequency = 0.0,
          const TSample &duty_cycle = 0.5,
          const PulsarWaveforms &waveform = SINE,
          const PulsarWindows &window = RECTANGULAR);

    bool set_sample_rate(const TSample &sample_rate);
    bool set_frequency(const TSample &frequency);
    bool set_duty_cycle(const TSample &duty_cycle);
    void set_waveform(const PulsarWaveforms &waveform);
    void set_window(const PulsarWindows &window);
    void reset();

    inline TSample run();

    inline TSample get_last_sample();

private:
    TSample frequency_;
    TSample duty_cycle_;
    TSample inv_duty_cycle_;
    TSample inv_sample_rate_;
    TSample half_sample_rate_;

    TSample wave_step_;
    TSample wave_ramp_;
    TSample step_;
    TSample ramp_;
    TSample harmonics_;
    bool gen_wave_;

    TSample output_;

    PulsarWaveforms waveform_;
    PulsarWindows window_;

    const TSample m_2pi_ = 2.0 * M_PI;
    const TSample inv_rand_max2_ = 2.0 * (1.0 / RAND_MAX);
};

template <class TSample>
Pulsar<TSample>::Pulsar(const TSample &sample_rate,
                        const TSample &frequency,
                        const TSample &duty_cycle,
                        const PulsarWaveforms &waveform,
                        const PulsarWindows &window)
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

    set_duty_cycle(duty_cycle);
    set_waveform(waveform);
    set_window(window);

    srand(time(0));
}

template <class TSample>
bool Pulsar<TSample>::set_sample_rate(const TSample &sample_rate)
{
    if (sample_rate > 0.0)
    {
        half_sample_rate_ = sample_rate * 0.5;
        inv_sample_rate_ = 1.0 / sample_rate;

        set_frequency(frequency_);

        return true;
    }

    std::cerr << "Pulsar::sample_rate must be greater than zero\n";

    return false;
}

template <class TSample>
bool Pulsar<TSample>::set_frequency(const TSample &frequency)
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
    wave_step_ = frequency_ * inv_sample_rate_ * inv_duty_cycle_;

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
bool Pulsar<TSample>::set_duty_cycle(const TSample &duty_cycle)
{
    duty_cycle_ = std::clamp(duty_cycle, 0.0, 1.0);

    if (duty_cycle_ > 0.0)
    {
        inv_duty_cycle_ = 1.0 / duty_cycle_;
    }
    else
    {
        inv_duty_cycle_ = 0.0;
    }

    wave_step_ = frequency_ * inv_sample_rate_ * inv_duty_cycle_;

    return true;
}

template <class TSample>
void Pulsar<TSample>::set_waveform(const PulsarWaveforms &waveform)
{
    waveform_ = waveform;
}

template <class TSample>
void Pulsar<TSample>::set_window(const PulsarWindows &window)
{
    window_ = window;
}

template <class TSample>
inline TSample Pulsar<TSample>::run()
{
    output_ = 0.0;

    ramp_ += step_;
    if (ramp_ > duty_cycle_)
    {
        gen_wave_ = false;
    }
    if (abs(ramp_) > 1.0)
    {
        while (abs(ramp_) > 1.0)
        {
            ramp_ -=  1.0 * copysign(1.0, ramp_);
        }
        
        wave_ramp_ =  ramp_ * inv_duty_cycle_;

        gen_wave_ = true;
    }

    if (gen_wave_)
    {
        wave_ramp_ += wave_step_;
        if (abs(wave_ramp_) > 1.0)
        {
            gen_wave_ = false;
            return output_;
        }

        switch (waveform_)
        {
            case SINE:
            output_ = sin(wave_ramp_ * m_2pi_);
            break;
            case SAW:
            for (TSample harmonic = 1.0; harmonic <= harmonics_; harmonic++)
            {
                output_ += sin((-wave_ramp_ + 0.5) * m_2pi_ * harmonic) / harmonic;
            }
            output_ *= 0.55;
            break;
            case SQUARE:
            for (TSample harmonic = 1.0; harmonic <= harmonics_; harmonic += 2)
            {
                output_ += sin(wave_ramp_ * m_2pi_ * harmonic) / harmonic;
            }
            output_ *= 1.07;
            break;
            case TRIANGLE:
            for (TSample harmonic = 1.0; harmonic <= harmonics_; harmonic += 2)
            {
                output_ += cos((wave_ramp_ + 0.75) * m_2pi_ * harmonic) / (harmonic * harmonic);
            }
            output_ *= 0.82;
            break;
            case NOISE:
            output_ = (rand() * inv_rand_max2_) - 1.0;
            break;
            case DC:
            output_ = 1.0;
            break;
            case PHASOR:
            output_ = wave_ramp_;
            break;
        }

        switch (window_)
        {    
            case HANN:
            output_ *= hann(wave_ramp_);
            break;
            case HAMMING:
            output_ *= hamming(wave_ramp_);
            break;
            case BLACKMAN:
            output_ *= blackman(wave_ramp_);
            break;
            case NUTTALL:
            output_ *= nuttall(wave_ramp_);
            break;
            case BLACKMANNUTTALL:
            output_ *= blackmannuttall(wave_ramp_);
            break;
            case BLACKMANHARRIS:
            output_ *= blackmanharris(wave_ramp_);
            break;
            case FLATTOP:
            output_ *= flattop(wave_ramp_);
            break;
            case BARTLETTHANN:
            output_ *= bartletthann(wave_ramp_);
            break;
        }
    }

    return output_;
}

template <class TSample>
inline TSample Pulsar<TSample>::get_last_sample()
{
    return output_;
}

#endif // PULSAR_H_
