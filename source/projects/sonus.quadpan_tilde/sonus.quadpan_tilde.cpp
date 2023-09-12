/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

class quadpan_tilde : public object<quadpan_tilde>, public sample_operator<3, 4>
{
public:
	MIN_DESCRIPTION {"Pan the input in a quadraphonic setup"};
	MIN_TAGS {"audio, routing"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.quadpanner"};

	inlet<>  in1 {this, "(signal) Input"};
	inlet<>  in_x {this, "(signal) X position, from left to right (0..1)"};
	inlet<>  in_y {this, "(signal) Y position, from front to rear (0..1)"};
	outlet<> out_fl {this, "(signal) Front left output", "signal"};
	outlet<> out_fr {this, "(signal) Front right output", "signal"};
	outlet<> out_rl {this, "(signal) Rear left output", "signal"};
	outlet<> out_rr {this, "(signal) Rear right output", "signal"};

	samples<4> operator()(sample input, sample x = 0.5, sample y = 0.5)
    {
        auto front_left = input * cos(x * M_PI_2) * cos(y * M_PI_2);
        auto front_right = input * cos((1.0 - x) * M_PI_2) * cos(y * M_PI_2);
        auto rear_left = input * cos(x * M_PI_2) * cos((1.0 - y) * M_PI_2);
        auto rear_right = input * cos((1.0 - x) * M_PI_2) * cos((1.0 - y) * M_PI_2);

		return { {front_left, front_right, rear_left, rear_right} };
	}
};

MIN_EXTERNAL(quadpan_tilde);
