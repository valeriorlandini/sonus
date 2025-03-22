/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

class wtp : public object<wtp>
{
public:
	MIN_DESCRIPTION {"Returns the frequencies of a scale tuned according to Young's \"The Well Tuned Piano\""};
	MIN_TAGS {"math, music"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {""};

	inlet<>  in {this, "(number) Frequency of the fundamental"};
	outlet<> out_1 {this, "(float) First step"};
	outlet<> out_2 {this, "(float) Second step"};
	outlet<> out_3 {this, "(float) Third step"};
	outlet<> out_4 {this, "(float) Fourth step"};
	outlet<> out_5 {this, "(float) Fifth step"};
	outlet<> out_6 {this, "(float) Sixth step"};
	outlet<> out_7 {this, "(float) Seventh step (perfect fifth)"};
	outlet<> out_8 {this, "(float) Eighth step"};
	outlet<> out_9 {this, "(float) Ninth step"};
	outlet<> out_10 {this, "(float) Tenth step"};
	outlet<> out_11 {this, "(float) Eleventh step"};

    message<> m_number
	{
		this,
		"number",
		"Frequency of the fundamental, which the other intervals are tuned to.",
        MIN_FUNCTION
		{
			calculate_intervals(number(args[0]));

			return {};
		}
    };

	void calculate_intervals(number fundamental)
	{
		out_1.send(fundamental * (567.0/512.0));
		out_2.send(fundamental * (9.0/8.0));
		out_3.send(fundamental * (147.0/128.0));
		out_4.send(fundamental * (21.0/16.0));
		out_5.send(fundamental * (1323.0/1024.0));
		out_6.send(fundamental * (189.0/128.0));
		out_7.send(fundamental * (3.0/2.0));
		out_8.send(fundamental * (49.0/32.0));
		out_9.send(fundamental * (7.0/4.0));
		out_10.send(fundamental * (441.0/256.0));
		out_11.send(fundamental * (63.0/32.0));
	}
};

MIN_EXTERNAL(wtp);
