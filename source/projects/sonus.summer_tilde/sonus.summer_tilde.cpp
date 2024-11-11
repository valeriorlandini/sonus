/// @filepos_
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <string>
#include <vector>

using namespace c74::min;

class summer_tilde : public object<summer_tilde>, public vector_operator<>
{
public:
	MIN_DESCRIPTION {"Instant signal mixer"};
	MIN_TAGS {"audio, routing"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.summer"};

	outlet<> out {this, "(signal) Output", "signal"};

	argument<int> inputs_arg
    {
        this,
        "inputs",
        "The number of input channels."
    };

    attribute<bool, threadsafe::no> scale
    {
        this,
        "scale",
        false,
        title {"Scale inputs"},
        description {"Scale inputs dividing each value by the number of inlets."}
    };

    summer_tilde(const atoms& args = {})
    {
        inputs_ = 2;

        if (!args.empty() && int(args[0]) >= 1)
        {
            inputs_ = std::min(int(args[0]), 257);
        }

	    for (auto i = 0; i < inputs_; ++i)
        {
            m_inlets.push_back(std::make_unique<inlet<>>(this, "(signal) Input " + std::to_string(i+1), "signal"));
        }
    }

	void operator()(audio_bundle input, audio_bundle output)
    {
		double scaler = 1.0;
        if (scale)
        {
            scaler /= inputs_;
        }

        for (auto i = 0; i < input.frame_count(); ++i)
        {
            output.samples(0)[i] = 0.0;
            
            for (auto ch = 0; ch < inputs_; ch++)
            {
                output.samples(0)[i] += input.samples(ch)[i] * scaler;
    	    }
        }
    }

private:
    std::vector<std::unique_ptr<inlet<>>> m_inlets;
    int inputs_;
};

MIN_EXTERNAL(summer_tilde);
