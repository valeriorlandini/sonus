/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "simpleosc.h"

using namespace c74::min;
using namespace soutel;

class rsosc_tilde : public object<rsosc_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Non bandlimited oscillator with changing random shape"};
	MIN_TAGS {"oscillators"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.nwosc~, sonus.vaosc~"};

	inlet<>  in_f {this, "(signal/float) Frequency"};
	inlet<>  in_c {this, "(float) Cycles with same shape"};
	outlet<> out {this, "(signal) Output", "signal"};
	outlet<> out_b {this, "(symbol) Current wave shape"};

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

	argument<number> frequency_arg
	{
		this,
		"frequency",
		"Oscillator frequency.",
        MIN_ARGUMENT_FUNCTION
		{
            osc_.set_frequency(double(arg));
        }
    };

	argument<number> cycles_arg
	{
		this,
		"cycles",
		"Oscillator cycles with the same shape.",
        MIN_ARGUMENT_FUNCTION
		{
            cycles = arg;
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
				osc_.set_frequency(double(args[0]));
			}
			else
			{
				cycles = long(args[0]);
			}

			return {};
		}
    };


	sample operator()(sample freq)
    {	
		double output = 0.0;

		if (in_f.has_signal_connection())
		{
			osc_.set_frequency(freq);
		}

		if (osc_.run())
		{
			++cycle_;

			if (cycle_ >= cycles)
			{
				shape_ = Waveforms(rand() % 4);
				cycle_ = 0;		
				switch (shape_)
				{
					case Waveforms::sine:
					out_b.send("sine");
					break;
					case Waveforms::saw:
					out_b.send("saw");
					break;
					case Waveforms::pulse:
					out_b.send("square");
					break;
					case Waveforms::triangle:
					out_b.send("triangle");
					break;
				}
			}
		}

		switch (shape_)
		{
			case Waveforms::sine:
			output = osc_.get_sine();
			break;
			case Waveforms::saw:
			output = osc_.get_saw();
			break;
			case Waveforms::pulse:
			output = osc_.get_pulse();
			break;
			case Waveforms::triangle:
			output = osc_.get_triangle();
			break;
		}

		return output;
	}

	private:
	enum class Waveforms
	{
    	sine,
		saw,
		pulse,
    	triangle
	};

	SimpleOsc<double> osc_;
	Waveforms shape_ = Waveforms::sine;
	unsigned long cycle_ = 0;
};

MIN_EXTERNAL(rsosc_tilde);
