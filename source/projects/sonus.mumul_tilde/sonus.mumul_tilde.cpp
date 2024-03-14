/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <string>
#include <vector>

using namespace c74::min;

class mumul_tilde : public object<mumul_tilde>, public vector_operator<>
{
public:
	MIN_DESCRIPTION {"Multplies all the inputs for the same value"};
	MIN_TAGS {"audio, math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.mumul"};

	argument<int> inputs_arg
    {
        this,
        "inputs",
        "The number of input channels."
    };

	argument<float> factor_arg
    {
        this,
        "factor",
        "The multiplication factor."
    };

    message<> m_number
	{
		this,
		"number",
		"Factor",
        MIN_FUNCTION
		{
			if (inlet == inputs_ - 1)
			{
				factor_ = float(args[0]);
			}

			return {};
		}
    };

    mumul_tilde(const atoms& args = {})
    {
        inputs_ = 2;
        outputs_ = 1;


        if (!args.empty() && int(args[0]) > 1)
        {
            inputs_ = std::min(int(args[0]) + 1, 257);
            outputs_ = inputs_ - 1;
        }

        if (args.size() > 1)
        {
            factor_ = float(args[1]);
        }

	    for (auto i = 0; i < inputs_; ++i)
        {
            if (i < inputs_ - 1)
            {
                m_inlets.push_back(std::make_unique<inlet<>>(this, "(signal) Input " + std::to_string(i+1), "signal"));
                m_outlets.push_back(std::make_unique<outlet<>>(this, "(signal) Output " + std::to_string(i+1), "signal"));
            }
            else
            {
                m_inlets.push_back(std::make_unique<inlet<>>(this, "(float/signal) Multiplication factor", "signal"));
            }
        }
    }

	void operator()(audio_bundle input, audio_bundle output)
    {
		std::vector<float> inputs_sample;
        
        for (auto i = 0; i < input.frame_count(); ++i)
        {
            inputs_sample.clear();

            if (m_inlets.at(inputs_ - 1)->has_signal_connection())
            {
                factor_ = input.samples(inputs_ - 1)[i];
            }

            for (auto ch = 0; ch < outputs_; ch++)
            {
                output.samples(ch)[i] = input.samples(ch)[i] * factor_;
            }
	    }
    }

private:
    int inputs_;
    int outputs_;
    float factor_ = 0.0f;
    std::vector<std::unique_ptr<inlet<>>> m_inlets;
    std::vector<std::unique_ptr<outlet<>>> m_outlets;
};

MIN_EXTERNAL(mumul_tilde);
