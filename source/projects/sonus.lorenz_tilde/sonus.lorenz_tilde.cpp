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
		call_next_values();
	}

	void call_next_values()
	{
		lorenz_.step();

		next_values_ = {std::clamp(lorenz_.get_x() * 0.04, -1.0, 1.0),
		                std::clamp(lorenz_.get_y() * 0.04, -1.0, 1.0),
						std::clamp((lorenz_.get_z() * 0.04) - 1.0, -1.0, 1.0)};
	}

	samples<3> operator()(sample freq)
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
        description {"Evolution speed (0-1)"},
        setter
		{
			MIN_FUNCTION
			{
				lorenz_.set_t(double(args[0]) * 0.02);
				return args;
        	}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> rho
	{
        this,
        "rho",
        28.0,
		range { 24.0, 30.0 },
        title {"Rho parameter (24-30)"},
		description {"Value for the rho parameter. The standard value is 28.0, when it is greater than 24.74 the system is chaotic."},
		setter
		{
			MIN_FUNCTION
			{
				lorenz_.set_rho(args[0]);
				return args;
			}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> sigma
	{
        this,
        "sigma",
        10.0,
		range { 1.0, 15.0 },
        title {"Sigma parameter (1-15)"},
		description {"Value for the sigma parameter. The standard value is 10.0."},
		setter
		{
			MIN_FUNCTION
			{
				lorenz_.set_sigma(args[0]);
				return args;
			}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> beta
	{
        this,
        "beta",
        2.6667,
		range { 1.0, 5.0 },
        title {"Beta parameter (1-5)"},
		description {"Value for the rho parameter. The standard value is 2.6667."},
		setter
		{
			MIN_FUNCTION
			{
				lorenz_.set_beta(args[0]);
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
			beta = 2.6667;
			sigma = 10.0;
			rho = 28.0;
			speed = 0.5;
			lorenz_.set_x(0.01);
			lorenz_.set_y(0.0);
			lorenz_.set_z(0.0);

			return {};
		}
	};

private:
	Lorenz<double> lorenz_;
	std::vector<double> next_values_ = {0.0, 0.0, 0.0};
};

MIN_EXTERNAL(lorenz_tilde);
