/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <string>
#include <vector>

using namespace c74::min;

template<class TSample>
struct fselect_info
{
    TSample gain_a;
    TSample gain_b;
    unsigned int a;
    unsigned int b;
};

class fselector_tilde : public object<fselector_tilde>, public vector_operator<>
{
public:
	MIN_DESCRIPTION {"Continuous selector from contiguous inputs"};
	MIN_TAGS {"audio, routing"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"selector~"};

	outlet<> out {this, "(signal) Output", "signal"};

	argument<int> inputs_arg
    {
        this,
        "inputs",
        "The number of input channels."
    };

    enum class mixranges : int { zero_one, last_inputs, enum_count };

    enum_map mixranges_range = {"From 0.0 to 1.0", "From 1 to last input"};

    attribute<mixranges> mixer_range
    {
        this,
        "range",
        mixranges::zero_one,
        mixranges_range,
        title {"Mixer range"},
        description {"The range within position operates."}
    };

    fselector_tilde(const atoms& args = {})
    {
        inputs_ = 3;

        if (!args.empty() && int(args[0]) > 1)
        {
            inputs_ = std::min(int(args[0]) + 1, 257);
        }

	    for (auto i = 0; i < inputs_; ++i)
        {
            if (i == 0)
            {
                m_inlets.push_back(std::make_unique<inlet<>>(this, "(signal) Position", "signal"));
            }
            else
            {
                m_inlets.push_back(std::make_unique<inlet<>>(this, "(signal) Input " + std::to_string(i), "signal"));
            }
        }
    }

	void operator()(audio_bundle input, audio_bundle output)
    {
		std::vector<float> inputs_sample;
        
        for (auto i = 0; i < input.frame_count(); ++i)
        {
            inputs_sample.clear();

            for (auto ch = 1; ch < inputs_; ch++)
            {
                inputs_sample.push_back(input.samples(ch)[i]);
            }
            
            set_position_(input.samples(0)[i]);

            output.samples(0)[i] = inputs_sample.at(info_.a) * info_.gain_a
                                   + inputs_sample.at(info_.b) * info_.gain_b;
	    }
    }

private:
    int inputs_;
    float position_ = 0.0;
    std::vector<std::unique_ptr<inlet<>>> m_inlets;
    fselect_info<float> info_;

    void set_position_(const float &position)
    {   
        switch (mixer_range)
		{
			case mixranges::last_inputs:
                position_ = std::clamp(position - 1.0f, 0.0f,
                                       (float)(inputs_) - 2.0f);
                break;
            case mixranges::zero_one:
                position_ = std::clamp(position * ((float)(inputs_) - 2.0f),
                                       0.0f, (float)(inputs_) - 2.0f);
                break;
        }

        info_.a = floor(position_);
        info_.b = ceil(position_);

        info_.gain_b = position_ - info_.a;
        info_.gain_a = 1.0 - info_.gain_b;
    }
};

MIN_EXTERNAL(fselector_tilde);
