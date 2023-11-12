/******************************************************************************
This code is released into public domain.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/

#ifndef BIQUAD_H_
#define BIQUAD_H_

#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum BQFilters
{
    LOWPASS,
    HIPASS,
    BANDPASS,
    BANDREJECT,
    ALLPASS
};

template <class TSample>
class Biquad
{
public:
    Biquad(const TSample &sample_rate = 44100.0,
           const TSample &cutoff = 11025.0, const TSample &q = 0.707,
           const BQFilters &type = LOWPASS);

    bool set_sample_rate(const TSample &sample_rate);
    bool set_cutoff(const TSample &cutoff);
    bool set_q(const TSample &q);
    bool set_type(const BQFilters &type);

    void clear();

    inline TSample run(const TSample &input);
    inline void run(const TSample &input, TSample &output);

    inline TSample get_last_sample();

private:
    TSample sample_rate_;
    TSample half_sample_rate_;
    TSample inv_sample_rate_;
    TSample cutoff_;
    TSample q_;

    BQFilters type_;

    TSample k_;

    TSample w_[3];
    TSample a1_, a2_;
    TSample b0_, b1_, b2_;

    TSample output_;
};

template <class TSample>
Biquad<TSample>::Biquad(const TSample &sample_rate, const TSample &cutoff,
                        const TSample &q, const BQFilters &type)
{
    if (sample_rate > 0.0)
    {
        sample_rate_ = sample_rate;
    }
    else
    {
        sample_rate_ = 44100.0;
    }

    inv_sample_rate_ = 1.0 / sample_rate_;
    half_sample_rate_ = sample_rate_ * 0.5;

    if (q >= 0.0)
    {
        q_ = q;
    }
    else
    {
        q_ = 0.707;
    }

    if (type >= LOWPASS && type <= ALLPASS)
    {
        type_ = type;
    }
    else
    {
        type_ = LOWPASS;
    }

    clear();

    if (cutoff > 0.0 && cutoff <= half_sample_rate_)
    {
        set_cutoff(cutoff);
    }
    else
    {
        set_cutoff(sample_rate_ * 0.25);
    }
}

template <class TSample>
bool Biquad<TSample>::set_sample_rate(const TSample &sample_rate)
{
    if (sample_rate > 0.0)
    {
        sample_rate_ = sample_rate;
        inv_sample_rate_ = 1.0 / sample_rate_;
        half_sample_rate_ = sample_rate_ * 0.5;

        if (cutoff_ > half_sample_rate_)
        {
            cutoff_ = half_sample_rate_;

            std::cerr << "Biquad::cutoff_ set equal to Biquad::half_sample_rate_\n";
        }

        return set_cutoff(cutoff_);
    }

    std::cerr << "Biquad::sample_rate_ must be greater than zero\n";

    return false;
}

template <class TSample>
bool Biquad<TSample>::set_cutoff(const TSample &cutoff)
{
    if (cutoff > 0.0 && cutoff <= half_sample_rate_)
    {
        cutoff_ = cutoff;

        k_ = tan(M_PI * cutoff_ * inv_sample_rate_);

        TSample kkq = k_ * k_ * q_;
        TSample kkm1 = (k_ * k_) - 1.0;

        a1_ = (2.0 * q_ * kkm1) / (kkq + k_ + q_);
        a2_ = (kkq - k_ + q_) / (kkq + k_ + q_);

        switch (type_)
        {
            case LOWPASS:
            b0_ = kkq / (kkq + k_ + q_);
            b1_ = 2.0 * b0_;
            b2_ = b0_;
            break;
            case HIPASS:
            b0_ = q_ / (kkq + k_ + q_);
            b1_ = -2.0 * b0_;
            b2_ = b0_;
            break;
            case BANDPASS:
            b0_ = k_ / (kkq + k_ + q_);
            b1_ = 0.0;
            b2_ = -1.0 * b0_;
            break;
            case BANDREJECT:
            b0_ = (q_ * (1.0 + (k_ * k_))) / (kkq + k_ + q_);
            b1_ = (2.0 * q_ * kkm1) / (kkq + k_ + q_);
            b2_ = b0_;
            break;
            case ALLPASS:
            b0_ = (kkq - k_ + q_) / (kkq + k_ + q_);
            b1_ = (2.0 * q_ * kkm1) / (kkq + k_ + q_);
            b2_ = 1.0;
            break;
        }

        return true;
    }

    std::cerr << "Biquad::cutoff_ must be inside (0, Biquad::half_sample_rate_] Hz\n";

    return false;
}

template <class TSample>
bool Biquad<TSample>::set_q(const TSample &q)
{
    if (q >= 0.0)
    {
        q_ = q;

        return set_cutoff(cutoff_);
    }

    std::cerr << "Biquad::q_ cannot be negative\n";

    return false;
}

template <class TSample>
bool Biquad<TSample>::set_type(const BQFilters &type)
{
    if (type >= LOWPASS && type <= ALLPASS)
    {
        type_ = type;

        return set_cutoff(cutoff_);
    }

    std::cerr << "Invalid filter type\n";
    
    return false;
}

template <class TSample>
void Biquad<TSample>::clear()
{
    w_[0] = 0.0;
    w_[1] = 0.0;
    w_[2] = 0.0;
}

template <class TSample>
inline TSample Biquad<TSample>::run(const TSample &input)
{
    w_[2] = w_[1];
    w_[1] = w_[0];
    w_[0] = input - a1_ * w_[1] - a2_ * w_[2];

    output_ = b0_ * w_[0] + b1_ * w_[1] + b2_ * w_[2];

    return output_;
}

template <class TSample>
inline void Biquad<TSample>::run(const TSample &input, TSample &output)
{
    output = run(input);
}

template <class TSample>
inline TSample Biquad<TSample>::get_last_sample()
{
    return output_;
}

#endif // BIQUAD_H_
