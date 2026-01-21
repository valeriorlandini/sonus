/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 (M_PI * 0.5)
#endif

using namespace c74::min;

class quadpanner : public object<quadpanner>
{
public:
	MIN_DESCRIPTION {"Returns the gains of a quadraphonic setup"};
	MIN_TAGS {"audio, routing"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.quadpan~"};

	inlet<>  in_x {this, "(float) X position, from left to right (0..1)"};
	inlet<>  in_y {this, "(float) Y position, from front to rear (0..1)"};
	outlet<> out_fl {this, "(float) Front left gain"};
	outlet<> out_fr {this, "(float) Front right gain"};
	outlet<> out_rl {this, "(float) Rear left gain"};
	outlet<> out_rr {this, "(float) Rear right gain"};

	argument<number> xpos_arg
	{
		this,
		"X position",
		"Initial X position, from left to right (0..1).",
        MIN_ARGUMENT_FUNCTION
		{
            x_pos = arg;
        }
    };

	argument<number> ypos_arg
	{
		this,
		"Y position",
		"Initial Y position, from front to rear (0..1).",
        MIN_ARGUMENT_FUNCTION
		{
            y_pos = arg;
        }
    };

    message<> m_number
	{
		this,
		"number",
		"Coordinate. Left to right on first inlet and front to rear on second inlet (0..1)",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				x_pos = args;
			}
			else
			{
				y_pos = args;
			}

			output(x_pos.get(), y_pos.get());
			return {};
		}
    };

	attribute<number, threadsafe::no, limit::clamp> x_pos
	{
        this,
        "x",
        0.5,
		range {0.0, 1.0},
        title {"X position"},
        description {"X position, from front to rear (0..1)."},
		setter
		{
			MIN_FUNCTION
			{
				output(args[0], y_pos.get());
				return args;
			}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> y_pos
	{
        this,
        "y",
        0.5,
		range {0.0, 1.0},
        title {"Y position"},
        description {"Y position, from front to rear (0..1)."},
		setter
		{
			MIN_FUNCTION
			{
				output(x_pos.get(), args[0]);
				return args;
			}
		}
    };

	void output(double x, double y)
	{
	    out_fl.send(cos(x * M_PI_2) * cos(y * M_PI_2));
    	out_fr.send(cos((1.0 - x) * M_PI_2) * cos(y * M_PI_2));
        out_rl.send(cos(x * M_PI_2) * cos((1.0 - y) * M_PI_2));
        out_rr.send(cos((1.0 - x) * M_PI_2) * cos((1.0 - y) * M_PI_2));
	}
};

MIN_EXTERNAL(quadpanner);
