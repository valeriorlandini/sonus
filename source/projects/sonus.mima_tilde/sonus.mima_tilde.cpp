/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

class mima_tilde : public object<mima_tilde>, public sample_operator<1, 6>
{
public:
	MIN_DESCRIPTION {"Reports the minimum and maximum value and delta ever received"};
	MIN_TAGS {"audio"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"minimum~, maximum~, minmax~"};

	inlet<>  in1 {this, "(signal) Input"};
	outlet<> out_amax {this, "(signal) Maximum (absolute value)", "signal"};
	outlet<> out_min {this, "(signal) Minimum", "signal"};
	outlet<> out_max {this, "(signal) Maximum", "signal"};
	outlet<> out_amaxd {this, "(signal) Maximum delta (absolute value)", "signal"};
	outlet<> out_maxd {this, "(signal) Maximum delta", "signal"};
	outlet<> out_mind {this, "(signal) Minimum delta", "signal"};
	
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
			amaxd_ = 0.0;
			maxd_ = 0.0;
			mind_ = 0.0;
			last_out_ = 0.0;
			last_set_ = false;

			return {};
		}
	};

	samples<6> operator()(sample input)
    {
		if (last_set_)
		{
			sample delta = input - last_out_;
			if (abs(delta) > amaxd_)
			{
				amaxd_ = abs(delta);
			}
			if (delta > maxd_)
			{
				maxd_ = delta;
			}
			if (delta < mind_)
			{
				mind_ = delta;
			}
		}
		else
		{
			last_set_ = true;		
		}
		last_out_ = input;

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

		return { {amax_, min_, max_, amaxd_, maxd_, mind_} };
	}

	private:
	sample amax_ = 0.0;
	sample min_ = 0.0;
	sample max_ = 0.0;
	sample amaxd_ = 0.0;
	sample maxd_ = 0.0;
	sample mind_ = 0.0;
	sample last_out_ = 0.0;
	bool last_set_ = false;
};

MIN_EXTERNAL(mima_tilde);
