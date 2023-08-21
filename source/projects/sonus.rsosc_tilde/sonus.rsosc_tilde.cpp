/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "vaosc.h"

using namespace c74::min;

class rsosc_tilde : public object<rsosc_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Oscillator with changing random change"};
	MIN_TAGS {"oscillators"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {""};

	inlet<>  in_f {this, "(signal/float) Frequency", frequency};
	inlet<>  in_c {this, "(float) Cycles with same shape"};
	outlet<> out {this, "(signal) Output", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			osc_.set_sample_rate(args[0]);
			return {};
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
				osc_.set_frequency(args[0]);
				return args;
			}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> cycles
	{
        this,
        "cycles",
        1,
		range {1, 65536},
        title {"Cycles with same shape"},
        description {"Oscillator cycles with the same shape."},
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
		"Frequency on first inlet, cycles before shape change on second inlet",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				frequency = args;
			}
			else
			{
				cycles = long(args[0]);
			}

			return {};
		}
    };


	sample operator()(sample x)
    {	
		double output = 0.0;

		if (osc_.run())
		{
			++cycle_;

			if (cycle_ >= cycles)
			{
				shape_ = amtl::VAWaveforms(rand() % 4);
				cycle_ = 0;
			}
		}

		switch (shape_)
		{
			case amtl::SINE:
			output = osc_.get_sine();
			break;
			case amtl::SAW:
			output = osc_.get_saw();
			break;
			case amtl::PULSE:
			output = osc_.get_pulse();
			break;
			case amtl::TRIANGLE:
			output = osc_.get_triangle();
			break;
		}

		return output;
	}

	private:
	amtl::VAOsc<double> osc_;
	amtl::VAWaveforms shape_ = amtl::SINE;
	unsigned long cycle_ = 0;
};

MIN_EXTERNAL(rsosc_tilde);
