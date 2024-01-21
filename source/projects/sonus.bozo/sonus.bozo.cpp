/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;

class bozo : public object<bozo>
{
public:
	MIN_DESCRIPTION {"Outputs a 1 when it receives a bang and then a 0 after a certain amount of time"};
	MIN_TAGS {"trigger"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"bang, trigger"};

	inlet<>  in {this, "(bang) Bang"};
	inlet<>  in_d {this, "(number) Milliseconds between 1 and 0"};
	outlet<> out {this, "(int) Output (1 then 0)"};

	timer<> metro
	{
		this,
        MIN_FUNCTION
		{
			if (send_zero_)
			{
				out.send(0);
				send_zero_ = false;
			}

			metro.stop();
            return {};
        }
    };

	argument<number> interval_arg
	{
		this,
		"interval",
		"Interval between 1 and 0 in ms.",
        MIN_ARGUMENT_FUNCTION
		{
            interval = arg;
        }
    };

	attribute<number, threadsafe::no, limit::clamp> interval
	{
		this,
		"interval",
		20.0,
		range { 1.0, 10000.0 },
        title {"1-0 interval (ms)"},
        description {"Interval between the outputs of 1 and 0, in milliseconds. Default is 20 ms."},
        setter
		{
			MIN_FUNCTION
			{
				return args;
        	}
		}
    };

	message<> bang
	{
		this,
		"bang",
		"Output 1 and then 0.",
        MIN_FUNCTION
		{
			out.send(1);
			
			send_zero_ = true;

			metro.delay(interval);

			return {};
		}
    };

	message<> number
	{
		this,
		"number",
		"On the second inlet, set the interval between 1 and 0 outputs.",
        MIN_FUNCTION
		{
			if (inlet == 1)
			{
				interval = args[0];
			}

			return {};
		}
    };

	private:
	bool send_zero_ = false;
};

MIN_EXTERNAL(bozo);
