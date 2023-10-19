#ifndef DELAY_H_
#define DELAY_H_

#include <algorithm>
#include <cmath>
#include <vector>
#include <iostream>

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

    int delay_samples_;

    TSample feedback_;

    TSample output_;

    int read_pos_;
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

    if (max_delay_time_ > 0.0)
    {
        buffer_.assign((unsigned int)ceil(max_delay_time_ * sample_rate_ * 0.001) + 1, 0.0);
    }
    else
    {
        buffer_.assign((unsigned int)ceil(5.0 * sample_rate_) + 1, 0.0);
    }

    read_pos_ = 0.0;
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

        delay_samples_ = (int)floor(sample_rate_ * delay_time_ * 0.001);

        read_pos_ = write_pos_ - delay_samples_;
        if (read_pos_ < 0)
        {
            read_pos_ = buffer_.size() + read_pos_;
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
    output_ = (buffer_.at(read_pos_));

    buffer_.at(write_pos_) = input + (output_ * feedback_);

    if (++write_pos_ >= buffer_.size())
    {
        write_pos_ = 0;
    }

    if (++read_pos_ >= buffer_.size())
    {
        read_pos_ = 0;
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
