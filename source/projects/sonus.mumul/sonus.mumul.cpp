/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <string>
#include <vector>

using namespace c74::min;

class mumul : public object<mumul>
{
public:
	MIN_DESCRIPTION {"Multplies all the inputs for the same value"};
	MIN_TAGS {"math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.mumul~"};

	argument<int> inputs_arg
    {
        this,
        "inputs",
        "The number of inputs."
    };

	argument<float> factor_arg
    {
        this,
        "factor",
        "The multiplication factor."
    };

    mumul(const atoms& args = {})
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
                m_inlets.push_back(std::make_unique<inlet<>>(this, "(number) Input " + std::to_string(i+1)));
                m_outlets.push_back(std::make_unique<outlet<>>(this, "(float) Output " + std::to_string(i+1)));
            }
            else
            {
                m_inlets.push_back(std::make_unique<inlet<>>(this, "(float) Multiplication factor"));
            }
        }

        in_values_.resize(inputs_ - 1, 0.0f);
    }

    message<> m_bang
	{
		this,
		"bang",
		"Output the current values",
        MIN_FUNCTION
		{
			calculation();
			return {};
		}
    };

    message<> m_number
	{
		this,
		"number",
		"Value to scale or, in the last inlet, scale factor",
        MIN_FUNCTION
		{
			if (inlet < inputs_ - 1)
			{
				in_values_.at(inlet) = float(args[0]);
			}
			else
			{
				factor_ = float(args[0]);
			}

			calculation();
			return {};
		}
    };

    message<> clear
    {
        this,
        "clear",
        "Clear the current values, without output",
        MIN_FUNCTION
        {
            for (auto &i: in_values_)
            {
                i = 0.0f;
            }
            return {};
        }
    };

    void calculation()
    {
        for (auto o = 0; o < m_outlets.size(); o++)
        {
            m_outlets.at(o)->send(in_values_.at(o) * factor_);
        }
    }

private:
    int inputs_;
    int outputs_;
    float factor_ = 0.0f;
    std::vector<std::unique_ptr<inlet<>>> m_inlets;
    std::vector<std::unique_ptr<outlet<>>> m_outlets;
    std::vector<float> in_values_;
};

MIN_EXTERNAL(mumul);
