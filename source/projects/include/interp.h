#include <algorithm>
#include <cmath>
#include <vector>

template <class TSample>
inline TSample cosip(const TSample &a, const TSample &b, const TSample &t)
{
    TSample interp = (1.0 - cos(t * M_PI)) * 0.5;

    return a * (1.0 - interp) + b * interp;
}

template <class TSample>
std::vector<TSample> resize_chunk(const std::vector<TSample> &chunk, const unsigned int &new_size)
{
    if (new_size == chunk.size())
    {
        return chunk;
    }
    TSample ratio = (TSample)std::max((int)chunk.size() - 1, 1) / (TSample)new_size;
    std::vector<TSample> output(new_size, 0.0);

    for (auto i = 1; i < new_size; i++)
    {
        TSample in_pos = (TSample)i * ratio;
        int in_a = (int)floor(in_pos);
        int in_b = (int)ceil(in_pos) % chunk.size();
        TSample in_t = in_pos - (TSample)in_a;
        output.at(i) = cosip(chunk.at(in_a), chunk.at(in_b), in_t);      
    }

    return output;
}