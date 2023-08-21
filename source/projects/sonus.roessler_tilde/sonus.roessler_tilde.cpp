/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "roessler.hpp"
#include <algorithm>
#include <vector>

using namespace c74::min;

template <class TSample>
inline TSample cosip(const TSample &a, const TSample &b, const TSample &t)
{
    TSample interp = (1.0 - cos(t * M_PI)) * 0.5;

    return a * (1.0 - interp) + b * interp;
}

class roessler_tilde : public object<roessler_tilde>, public sample_operator<1, 3>
{
public:
	MIN_DESCRIPTION {"Roessler system based oscillator"};
	MIN_TAGS {"strange attractors, oscillator"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.roessler"};

	inlet<>  in {this, "(signal) Frequency"};
	outlet<> out_x {this, "(signal) x component", "signal"};
	outlet<> out_y {this, "(signal) y component", "signal"};
	outlet<> out_z {this, "(signal) z component", "signal"};

	roessler_tilde()
	{
		call_next_value();
		set_frequency(0.0);
	}

	void set_frequency(const double &freq)
	{
		double hsr = (samplerate() * 0.5);

		if (hsr != 0.0)
		{
			step_length_ = std::abs(std::fmod(freq, hsr)) / hsr;
		}
		else
		{
			step_length_ = 0.0;
		}
	}

	void call_next_value()
	{
		current_points_ = next_points_;

		roessler_.step();

		next_points_ = {std::clamp(roessler_.get_x() * 0.08, -1.0, 1.0),
		                std::clamp(roessler_.get_y() * 0.08, -1.0, 1.0),
						std::clamp((roessler_.get_z() * 0.04) - 1.0, -100.0, 100.0)};
	}

	samples<3> operator()(sample freq)
    {
		if (freq != frequency)
		{
			frequency = freq;
		}
		
		ramp_ += step_length_;
		if (ramp_ > 1.0)
		{
			ramp_ = fmod(ramp_, 1.0);
			call_next_value();
		}

		auto x = cosip(current_points_[0], next_points_[0], ramp_);
		auto y = cosip(current_points_[1], next_points_[1], ramp_);
		auto z = cosip(current_points_[2], next_points_[2], ramp_);

		return { {x, y, z} };
	}

    message<> m_number
	{
		this,
		"number",
		"Set the frequency in Hz.",
        MIN_FUNCTION
		{
            frequency = args;
            return {};
        }
    };

	argument<number> frequency_arg
    {
        this,
        "frequency",
        "Frequency in Hz.",
        MIN_ARGUMENT_FUNCTION
        {
            frequency = arg;
        }
    };

	attribute<number> frequency {
		this,
		"frequency",
		0.0,
        description {"Frequency in Hz"},
        setter
		{
			MIN_FUNCTION
			{
				set_frequency(args[0]);
				return args;
        	}
		}
    };

	attribute<number> a
	{
        this,
        "a",
        0.2,
        title {"a parameter"},
		description {"Value for the a parameter. The standard value is 0.2."},
		setter
		{
			MIN_FUNCTION
			{
				roessler_.set_a(args[0]);

				return {};
			}
		}
    };

	attribute<number> b
	{
        this,
        "b",
        0.2,
        title {"b parameter"},
		description {"Value for the b parameter. The standard value is 0.2."},
		setter
		{
			MIN_FUNCTION
			{
				roessler_.set_b(args[0]);

				return {};
			}
		}
    };

	attribute<number> c
	{
        this,
        "c",
        5.7,
        title {"c parameter"},
		description {"Value for the c parameter. The standard value is 5.7."},
		setter
		{
			MIN_FUNCTION
			{
				roessler_.set_c(args[0]);

				return {};
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
			roessler_.set_a(0.2);
			roessler_.set_b(0.2);
			roessler_.set_c(5.7);
			roessler_.set_x(0.1);
			roessler_.set_y(0.1);
			roessler_.set_z(0.1);
			call_next_value();

			return {};
		}
	};

private:
	Roessler<double> roessler_;
	double step_length_;
	double ramp_ = 0.0;
	std::vector<double> current_points_ = {0.0, 0.0, 0.0};
	std::vector<double> next_points_ = {0.0, 0.0, 0.0};
};

MIN_EXTERNAL(roessler_tilde);
