/// @filepos_
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <string>
#include <vector>

using namespace c74::min;

class summer : public object<summer>
{
public:
	MIN_DESCRIPTION {"Hot numbers summator"};
	MIN_TAGS {"math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.summer~"};

	outlet<> out {this, "(float) Output"};

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

    summer(const atoms& args = {})
    {
        inputs_ = 2;

        if (!args.empty() && int(args[0]) >= 1)
        {
            inputs_ = std::min(int(args[0]), 257);
        }

	    for (auto i = 0; i < inputs_; ++i)
        {
            m_inlets.push_back(std::make_unique<inlet<>>(this, "(number) Input " + std::to_string(i+1)));
        }

        numbers_.resize(inputs_, 0.0f);
    }

    message<> m_number
	{
		this,
		"number",
		"Number to add",
        MIN_FUNCTION
		{
            numbers_.at(int(inlet)) = float(args[0]);

            sum();

			return {};
		}
    };

	message<> m_bang
	{
		this,
		"bang",
		"Output current sum",
		MIN_FUNCTION
		{
            sum();
		
        	return {};
		}
	};

    void sum()
    {
        float sum = 0.0f;

        for (float n : numbers_)
        {
            sum += n;
        }

        if (scale)
        {
            sum /= float(inputs_);
        }

        out.send(sum);
    }

private:
    std::vector<std::unique_ptr<inlet<>>> m_inlets;
    std::vector<float> numbers_;
    int inputs_;
};

MIN_EXTERNAL(summer);
