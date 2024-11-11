/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <vector>
#include "interp.h"
#include "roessler.h"

using namespace c74::min;
using namespace soutel;

class roessler_tilde : public object<roessler_tilde>, public sample_operator<0, 3>
{
public:
	MIN_DESCRIPTION {"Roessler system based generator"};
	MIN_TAGS {"strange attractors, oscillator"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.lorenz~"};

	inlet<>  in {this, "(float) Evolution speed"};
	outlet<> out_x {this, "(signal) x component", "signal"};
	outlet<> out_y {this, "(signal) y component", "signal"};
	outlet<> out_z {this, "(signal) z component", "signal"};

	void call_next_values()
	{
		roessler_.step();

		next_values_ = {std::clamp(roessler_.get_x() * 0.08, -1.0, 1.0),
		                std::clamp(roessler_.get_y() * 0.08, -1.0, 1.0),
						std::clamp((roessler_.get_z() * 0.02), -1.0, 1.0)};
	}

	samples<3> operator()()
    {
		call_next_values();

		return { {next_values_[0], next_values_[1], next_values_[2]} };
	}

    message<> m_number
	{
		this,
		"number",
		"Set the evolution speed (0-1).",
        MIN_FUNCTION
		{
            speed = args;
            return {};
        }
    };

	argument<number> speed_arg
    {
        this,
        "speed",
        "Evolution speed (0-1).",
        MIN_ARGUMENT_FUNCTION
        {
            speed = arg;
        }
    };

	attribute<number, threadsafe::no, limit::clamp> speed
	{
		this,
		"speed",
		0.5,
		range { 0.0, 1.0 },
        title {"Evolution speed (0-1)"},
        description {"Speed of evolution of the system, normalized between 0 and 1."},
        setter
		{
			MIN_FUNCTION
			{
				roessler_.set_t(double(args[0]) * 0.05);
				return args;
        	}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> a
	{
        this,
        "a",
        0.2,
		range { 0.1, 0.3 },
        title {"a parameter (0.1-0.3)"},
		description {"Value for the a parameter. The standard value is 0.2."},
		setter
		{
			MIN_FUNCTION
			{
				roessler_.set_a(args[0]);

				return args;
			}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> b
	{
        this,
        "b",
        0.2,
		range { 0.01, 1.0 },
        title {"b parameter (0.01-1)"},
		description {"Value for the b parameter. The standard value is 0.2."},
		setter
		{
			MIN_FUNCTION
			{
				roessler_.set_b(args[0]);

				return args;
			}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> c
	{
        this,
        "c",
        5.7,
		range { 2.0, 10.0 },
        title {"c parameter (2-10)"},
		description {"Value for the c parameter. The standard value is 5.7."},
		setter
		{
			MIN_FUNCTION
			{
				roessler_.set_c(args[0]);

				return args;
			}
		}
    };

	message<> clear
	{
		this,
        "clear",
        "Reset parameters and initial values to the defaults",
        MIN_FUNCTION
        {
			a = 0.2;
			b = 0.2;
			c = 5.7;
			speed = 0.5;
			roessler_.set_x(0.1);
			roessler_.set_y(0.1);
			roessler_.set_z(0.1);

			return {};
		}
	};

private:
	Roessler<double> roessler_;
	std::vector<double> next_values_ = {0.0, 0.0, 0.0};
};

MIN_EXTERNAL(roessler_tilde);
