/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../include/lorenz.hpp"
#include <algorithm>
#include <vector>

using namespace c74::min;

template <class TSample>
inline TSample cosip(const TSample &a, const TSample &b, const TSample &t)
{
    TSample interp = (1.0 - cos(t * M_PI)) * 0.5;

    return a * (1.0 - interp) + b * interp;
}

class lorenz_tilde : public object<lorenz_tilde>, public sample_operator<1, 3>
{
public:
	MIN_DESCRIPTION {"Lorenz system based oscillator"};
	MIN_TAGS {"strange attractors, oscillator"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.lorenz"};

	inlet<>  in {this, "(signal) Frequency"};
	outlet<> out_x {this, "(signal) x component", "signal"};
	outlet<> out_y {this, "(signal) y component", "signal"};
	outlet<> out_z {this, "(signal) z component", "signal"};

	lorenz_tilde()
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

		lorenz_.step();

		next_points_ = {std::clamp(lorenz_.get_x() * 0.04, -1.0, 1.0),
		                std::clamp(lorenz_.get_y() * 0.04, -1.0, 1.0),
						std::clamp((lorenz_.get_z() * 0.04) - 1.0, -1.0, 1.0)};
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

	attribute<number> rho
	{
        this,
        "rho",
        28.0,
        title {"Rho parameter"},
		description {"Value for the rho parameter. The standard value is 28.0, when it is greater than 24.74 the system is chaotic."},
		setter
		{
			MIN_FUNCTION
			{
				lorenz_.set_rho(args[0]);

				return {};
			}
		}
    };

	attribute<number> sigma
	{
        this,
        "sigma",
        10.0,
        title {"Sigma parameter"},
		description {"Value for the sigma parameter. The standard value is 10.0."},
		setter
		{
			MIN_FUNCTION
			{
				lorenz_.set_sigma(args[0]);

				return {};
			}
		}
    };

	attribute<number> beta
	{
        this,
        "beta",
        2.6667,
        title {"Beta parameter"},
		description {"Value for the rho parameter. The standard value is 2.6667."},
		setter
		{
			MIN_FUNCTION
			{
				lorenz_.set_beta(args[0]);

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
			lorenz_.set_beta(2.6667);
			lorenz_.set_sigma(10.0);
			lorenz_.set_rho(28.0);
			lorenz_.set_x(0.01);
			lorenz_.set_y(0.0);
			lorenz_.set_z(0.0);
			call_next_value();

			return {};
		}
	};

private:
	Lorenz<double> lorenz_;
	double step_length_;
	double ramp_ = 0.0;
	std::vector<double> current_points_ = {0.0, 0.0, 0.0};
	std::vector<double> next_points_ = {0.0, 0.0, 0.0};
};

MIN_EXTERNAL(lorenz_tilde);
