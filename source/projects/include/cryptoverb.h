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

#ifndef CRYPTOVERB_H_
#define CRYPTOVERB_H_

#include <array>
#include <iostream>

#include "allpass.h"
#include "biquad.h"
#include "comb.h"
#include "vaosc.h"

template <class TSample>
struct block_one
{
    std::array<Comb<TSample>, 4> left = {
        Comb<TSample>(44100.0, 300.0, 277.0, 0.53, 0.45, 0.33),
        Comb<TSample>(44100.0, 30.0, 11.0, 0.67, -0.57, 0.77),
        Comb<TSample>(44100.0, 30.0, 13.0, 0.13, -0.71, 0.65),
        Comb<TSample>(44100.0, 300.0, 293.0, 0.64, 0.76, -0.75)
    };

    std::array<Comb<TSample>, 4> right = {
        Comb<TSample>(44100.0, 300.0, 233.0, 0.53, 0.33, 0.45),
        Comb<TSample>(44100.0, 30.0, 29.0, 0.67, 0.57, -0.77),
        Comb<TSample>(44100.0, 30.0, 5.0, 0.13, 0.71, -0.65),
        Comb<TSample>(44100.0, 300.0, 289.0, 0.64, -0.76, 0.75)
    };

    Biquad<TSample> lowpass_l{Biquad<TSample>(44100.0, 15000.0)};
    Biquad<TSample> lowpass_r{Biquad<TSample>(44100.0, 15000.0)};
};

template <class TSample>
struct block_two
{
    std::array<Comb<TSample>, 4> left = {
        Comb<TSample>(44100.0, 700.0, 677.0, 0.53, 0.45, 0.33),
        Comb<TSample>(44100.0, 1200.0, 1117.0, 0.67, -0.55, 0.77),
        Comb<TSample>(44100.0, 3000.0, 2999.0, 0.13, -0.71, 0.65),
        Comb<TSample>(44100.0, 900.0, 797.0, 0.64, 0.66, -0.65)
    };

    std::array<Comb<TSample>, 4> right = {
        Comb<TSample>(44100.0, 700.0, 691.0, 0.53, 0.33, 0.45),
        Comb<TSample>(44100.0, 1200.0, 1129.0, 0.65, 0.57, -0.77),
        Comb<TSample>(44100.0, 3000.0, 2791.0, 0.13, 0.71, -0.65),
        Comb<TSample>(44100.0, 900.0, 877.0, 0.64, -0.66, 0.65)
    };

    Biquad<TSample> lowpass_l{Biquad<TSample>(44100.0, 15000.0)};
    Biquad<TSample> lowpass_r{Biquad<TSample>(44100.0, 15000.0)};
};

template <class TSample>
struct block_three
{
    std::array<Allpass<TSample>, 4> left = {
        Allpass<TSample>(44100.0, 10.0, 7.0, 0.67),
        Allpass<TSample>(44100.0, 100.0, 97.0, -0.7),
        Allpass<TSample>(44100.0, 100.0, 91.0, 0.7),
        Allpass<TSample>(44100.0, 20.0, 19.0, 0.7)
    };

    std::array<Allpass<TSample>, 4> right = {
        Allpass<TSample>(44100.0, 10.0, 3.0, -0.67),
        Allpass<TSample>(44100.0, 100.0, 73.0, 0.7),
        Allpass<TSample>(44100.0, 100.0, 97.0, 0.7),
        Allpass<TSample>(44100.0, 20.0, 17.0, 0.7)
    };

    VAOsc<TSample> lfo_l{VAOsc<TSample>(44100.0, 0.19)};
    VAOsc<TSample> lfo_r{VAOsc<TSample>(44100.0, 0.17)};
};

template <class TSample>
class Cryptoverb
{
    public:
    Cryptoverb(const TSample &sample_rate = 44100.0,
               const TSample &block_one_wet = 1.0,
               const TSample &block_two_wet = 1.0,
               const TSample &block_three_wet = 1.0,
               const TSample &block_four_wet = 1.0);

    bool set_sample_rate(const TSample &sample_rate = 44100.0);
    bool set_block_wet(const TSample &wet = 1.0, const unsigned int &block = 1);

    inline std::array<TSample, 2> run(const TSample &input_l, const TSample &input_r);

    private:
    TSample sample_rate_;
    TSample output_l_;
    TSample output_r_;

    block_one<TSample> block_one_;
    block_two<TSample> block_two_;
    block_three<TSample> block_three_;

    TSample block_one_wet_;
    TSample block_two_wet_;
    TSample block_three_wet_;
    TSample block_four_wet_;

    std::array<TSample, 2> run_block_one_(const TSample &input_l, const TSample &input_r);
    std::array<TSample, 2> run_block_two_(const TSample &input_l, const TSample &input_r);
    std::array<TSample, 2> run_block_three_(const TSample &input_l, const TSample &input_r);

};

template <class TSample>
Cryptoverb<TSample>::Cryptoverb(const TSample &sample_rate,
                                const TSample &block_one_wet,
                                const TSample &block_two_wet,
                                const TSample &block_three_wet,
                                const TSample &block_four_wet)
{
    if (sample_rate > 0.0)
    {
        sample_rate_ = sample_rate;
    }
    else
    {
        sample_rate_ = 44100.0;
    }

    set_sample_rate(sample_rate_);
    set_block_wet(block_one_wet, 1);
    set_block_wet(block_two_wet, 2);
    set_block_wet(block_three_wet, 3);
    set_block_wet(block_four_wet, 4);

}

template <class TSample>
bool Cryptoverb<TSample>::set_sample_rate(const TSample &sample_rate)
{
    if (sample_rate > 0.0)
    {
        sample_rate_ = sample_rate;

        for (auto i = 0; i < 4; i++)
        {
            block_one_.left[i].set_sample_rate(sample_rate_);
            block_one_.right[i].set_sample_rate(sample_rate_);

            block_two_.left[i].set_sample_rate(sample_rate_);
            block_two_.right[i].set_sample_rate(sample_rate_);

            block_three_.left[i].set_sample_rate(sample_rate_);
            block_three_.right[i].set_sample_rate(sample_rate_);
        }

        block_one_.lowpass_l.set_sample_rate(sample_rate_);
        block_one_.lowpass_r.set_sample_rate(sample_rate_);
        block_two_.lowpass_l.set_sample_rate(sample_rate_);
        block_two_.lowpass_r.set_sample_rate(sample_rate_);
        block_three_.lfo_l.set_sample_rate(sample_rate_);
        block_three_.lfo_r.set_sample_rate(sample_rate_);

        return true;
    }

    std::cerr << "Cryptoverb::sample_rate_ must be greater than zero\n";

    return false;
}

template <class TSample>
bool Cryptoverb<TSample>::set_block_wet(const TSample &wet, const unsigned int &block)
{
    switch (block)
    {
        case 1:
        block_one_wet_ = std::clamp(wet, 0.0, 1.0);
        break;
        case 2:
        block_two_wet_ = std::clamp(wet, 0.0, 1.0);
        break;
        case 3:
        block_three_wet_ = std::clamp(wet, 0.0, 1.0);
        break;
        case 4:
        block_four_wet_ = std::clamp(wet, 0.0, 1.0);
        break;
    }

    return true;
}


template <class TSample>
inline std::array<TSample, 2> Cryptoverb<TSample>::run(const TSample &input_l, const TSample &input_r)
{
    std::array<TSample, 2> output_1 = run_block_one_(input_l, input_r);
    TSample input_l2 = output_1[0] * block_one_wet_ + (1.0 - block_one_wet_) * input_l;
    TSample input_r2 = output_1[1] * block_one_wet_ + (1.0 - block_one_wet_) * input_r;
    std::array<TSample, 2> output_2 = run_block_two_(input_l2, input_r2);
    TSample input_l3 = output_2[0] * block_two_wet_ + (1.0 - block_two_wet_) * input_l2;
    TSample input_r3 = output_2[1] * block_two_wet_ + (1.0 - block_two_wet_) * input_r2;
    std::array<TSample, 2> output_3 = run_block_three_(input_l3, input_r3);

    output_l_ = output_3[0] * block_three_wet_ + (1.0 - block_three_wet_) * input_l3;
    output_r_ = output_3[1] * block_three_wet_ + (1.0 - block_three_wet_) * input_r3;

    std::array<TSample, 2> outputs = {output_l_, output_r_};

    return outputs;  
}

template <class TSample>
std::array<TSample, 2> Cryptoverb<TSample>::run_block_one_(const TSample &input_l, const TSample &input_r)
{
    TSample out_l1 = block_one_.left[0].run(input_l);
    out_l1 = block_one_.left[1].run(out_l1);

    TSample out_l2 = block_one_.left[2].run(out_l1);
    out_l2 = block_one_.left[3].run(out_l2);

    TSample out_r1 = block_one_.right[0].run(input_r);
    out_r1 = block_one_.right[1].run(out_r1);

    TSample out_r2 = block_one_.right[2].run(out_r1);
    out_r2 = block_one_.right[3].run(out_r2);

    TSample out_l = (out_l1 - out_l2) + (out_r1 - out_r2);
    out_l *= 0.707;
    out_l = block_one_.lowpass_l.run(out_l);

    TSample out_r = (out_l1 + out_l2) + (out_r1 + out_r2);
    out_r *= 0.707;
    out_r = block_one_.lowpass_r.run(out_r);

    return std::array<TSample, 2> {out_l, out_r};  
}

template <class TSample>
std::array<TSample, 2> Cryptoverb<TSample>::run_block_two_(const TSample &input_l, const TSample &input_r)
{
    TSample out_l1 = block_two_.left[0].run(input_l);
    out_l1 = block_two_.left[1].run(out_l1);

    TSample out_l2 = block_two_.left[2].run(out_l1);
    out_l2 = block_two_.left[3].run(out_l2);

    TSample out_r1 = block_two_.right[0].run(input_r);
    out_r1 = block_two_.right[1].run(out_r1);

    TSample out_r2 = block_two_.right[2].run(out_r1);
    out_r2 = block_two_.right[3].run(out_r2);

    TSample out_l = (out_l1 - out_l2) + (out_r1 - out_r2);
    out_l *= 0.707;
    out_l = block_two_.lowpass_l.run(out_l);

    TSample out_r = (out_l1 + out_l2) + (out_r1 + out_r2);
    out_r *= 0.707;
    out_r = block_two_.lowpass_r.run(out_r);  

    return std::array<TSample, 2> {out_l, out_r};  
}

template <class TSample>
std::array<TSample, 2> Cryptoverb<TSample>::run_block_three_(const TSample &input_l, const TSample &input_r)
{
    TSample out_l = block_three_.left[0].run(input_l);
    out_l = block_three_.left[1].run(out_l);
    out_l = block_three_.left[2].run(out_l);
    out_l = block_three_.left[3].run(out_l);

    TSample out_r = block_three_.right[0].run(input_r);
    out_r = block_three_.right[1].run(out_r);
    out_r = block_three_.right[2].run(out_r);
    out_r = block_three_.right[3].run(out_r);
    
    block_three_.lfo_l.run();
    TSample lfo_l_out = block_three_.lfo_l.get_sine();
    lfo_l_out = ((lfo_l_out + 1.0) * 10.0) + 77.0;
    block_three_.left[2].set_time(lfo_l_out);

    block_three_.lfo_r.run();
    TSample lfo_r_out = block_three_.lfo_r.get_sine();
    lfo_r_out = ((lfo_r_out + 1.0) * 10.0) + 76.0;
    block_three_.right[2].set_time(lfo_r_out);

    return std::array<TSample, 2> {out_l, out_r};  
}

#endif // CRYPTOVERB_H_