/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "pulsar.h"

using namespace c74::min;

class pulsar_tilde : public object<pulsar_tilde>, public sample_operator<2, 1>
{
public:
	MIN_DESCRIPTION {"Pulsar oscillator"};
	MIN_TAGS {"oscillators"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.rsosc~, saw~, tri~, cycle~, rect~, noise~"};

	inlet<>  in_f {this, "(signal/float) Frequency"};
	inlet<>  in_dc {this, "(signal/float) Duty cycle"};
	outlet<> out {this, "(signal) Output", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			pulsar_.set_sample_rate(args[0]);
			return {};
		}
	};

	argument<number> frequency_arg
	{
		this,
		"frequency",
		"Oscillator frequency.",
        MIN_ARGUMENT_FUNCTION
		{
            frequency = arg;
        }
    };

	argument<number> duty_cycle_arg
	{
		this,
		"duty cycle",
		"Duty cycle (0-1).",
        MIN_ARGUMENT_FUNCTION
		{
            duty_cycle = arg;
        }
    };

	attribute<number, threadsafe::no> frequency
	{
        this,
        "frequency",
        0,
        title {"Frequency"},
        description {"Oscillator frequency."},
		setter
		{
			MIN_FUNCTION
			{
				pulsar_.set_frequency(args[0]);
				return args;
			}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> duty_cycle
	{
        this,
        "duty cycle",
        0.5,
		range {0.0, 1.0},
        title {"Duty cycle"},
        description {"Duty cycle of the wave (0-1)."},
		setter
		{
			MIN_FUNCTION
			{
				pulsar_.set_duty_cycle(args[0]);
				return args;
			}
		}
    };

	attribute<bool, threadsafe::no> normalize
	{
        this,
        "normalize",
        true,
        title {"Normalize gains"},
        description {"Whether or not normalize amounts of various shapes so that they always sum to 1 (unless they are all 0)."},
		setter
		{
			MIN_FUNCTION
			{
				return args;
			}
		}
    };

	message<> m_number
	{
		this,
		"number",
		"Depends on the inlet",
        MIN_FUNCTION
		{
			switch (inlet)
			{
				case 0:
				frequency = args;
				break;
				case 1:
				duty_cycle = args;
				break;
			}

			return {};
		}
    };

	sample operator()(sample freq, sample dc)
    {	
		if (in_f.has_signal_connection())
			frequency = freq;

		if (in_dc.has_signal_connection())
			duty_cycle = dc;

		return pulsar_.run();
	}

	private:
	Pulsar<double> pulsar_;
};

MIN_EXTERNAL(pulsar_tilde);
