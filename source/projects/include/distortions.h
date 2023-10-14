#ifndef DISTORTIONS_H_
#define DISTORTIONS_H_

template <class TSample>
inline TSample symmetrical_soft_clip(const TSample &sample, const TSample &threshold = 0.3333)
{
    TSample abs_sample = std::abs(sample);
    TSample sample_out = 0.0;

    if (abs_sample < threshold)
    {
        sample_out = sample * 2.0;
    }
    else if (abs_sample >= threshold && abs_sample < (2.0 * threshold))
    {
        sample_out = (3.0 - std::pow((2.0 - 3.0 * abs_sample), 2.0)) / std::copysign(3.0, sample);
    }
    else
    {
        sample_out = std::copysign(1.0, sample);
    }

    return sample_out;
}

template <class TSample>
inline TSample exponential_distortion(const TSample &sample, const TSample &gain, const TSample &mix = 1.0)
{
    TSample q = sample * gain;
    TSample z = std::copysign(1.0, q) * (1.0 - std::exp(-1.0 * std::abs(q)));

    return mix * z + (1.0 - mix) * sample;
}

#endif // DISTORTIONS_H_
