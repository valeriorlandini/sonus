/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "pdosc.h"

using namespace c74::min;
using namespace soutel;

class pdosc_tilde : public object<pdosc_tilde>, public sample_operator<2, 1>
{
public:
	MIN_DESCRIPTION {"Phase distortion based oscillator"};
	MIN_TAGS {"oscillators"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.nwosc~, sonus.vaosc~"};

	inlet<>  in_f {this, "(signal/float) Frequency"};
	inlet<>  in_d {this, "(signal/float) Phase distortion amount"};
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

	argument<number> d_arg
	{
		this,
		"d",
		"d value.",
        MIN_ARGUMENT_FUNCTION
		{
            osc_.set_d((double(arg) + 1.0) * 0.5);
        }
    };

	message<> m_number
	{
		this,
		"number",
		"Frequency on first inlet, d value on second inlet",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				osc_.set_frequency(double(args[0]));
			}
			else
			{
            	osc_.set_d((double(args[0]) + 1.0) * 0.5);
			}

			return {};
		}
    };

	message<> reset
    {
        this,
        "reset",
        "Reset the phase",
        MIN_FUNCTION
        {
            osc_.reset();

			return {};
		}
	};

	sample operator()(sample freq, sample d)
    {	
		double output = 0.0;

		if (in_f.has_signal_connection())
		{
			osc_.set_frequency(freq);
		}
		if (in_d.has_signal_connection())
		{
			osc_.set_d((d + 1.0) * 0.5);
		}

		return { osc_.run() };
	}

	private:
	PDOsc<double> osc_;
};

MIN_EXTERNAL(pdosc_tilde);
