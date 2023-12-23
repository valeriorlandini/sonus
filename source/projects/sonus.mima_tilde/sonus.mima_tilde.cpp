/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

class mima_tilde : public object<mima_tilde>, public sample_operator<1, 3>
{
public:
	MIN_DESCRIPTION {"Reports the minimum and maximum value ever received"};
	MIN_TAGS {"audio"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"minimum~, maximum~"};

	inlet<>  in1 {this, "(signal) Input"};
	outlet<> out_amax {this, "(signal) Maximum (absolute value)", "signal"};
	outlet<> out_min {this, "(signal) Minimum", "signal"};
	outlet<> out_max {this, "(signal) Maximum", "signal"};
	
	message<> clear
	{
		this,
        "clear",
        "Reset recorded values",
        MIN_FUNCTION
        {
			amax_ = 0.0;
			min_ = 0.0;
			max_ = 0.0;

			return {};
		}
	};

	samples<3> operator()(sample input)
    {
        if (abs(input) > amax_)
		{
			amax_ = abs(input);
		}
        if (input > max_)
		{
			max_ = input;
		}
        if (input < min_)
		{
			min_ = input;
		}

		return { {amax_, min_, max_} };
	}

	private:
	sample amax_ = 0.0;
	sample min_ = 0.0;
	sample max_ = 0.0;
};

MIN_EXTERNAL(mima_tilde);
