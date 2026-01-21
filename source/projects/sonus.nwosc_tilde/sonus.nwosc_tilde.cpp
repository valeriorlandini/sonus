/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "interp.h"

using namespace c74::min;
using namespace soutel;

class nwosc_tilde : public object<nwosc_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Wavetable oscillator with user defined table"};
	MIN_TAGS {"oscillators"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.rsosc~, sonus.vaosc~"};

	inlet<>  in {this, "(signal/float) Frequency"};
	outlet<> out {this, "(signal) Output", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			sample_rate_ = args[0];
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
            step_ = double(arg) / sample_rate_;
        }
    };

	attribute<std::vector<double>> wavetable
	{
		this,
		"wavetable",
		{-1.0, 1.0},
		title {"Wavetable"},
		description {"The values of the wavetable."}
	};

	message<> m_number
	{
		this,
		"number",
		"Oscillator frequency",
        MIN_FUNCTION
		{
			step_ = double(args[0]) / sample_rate_;
			return {};
		}
    };

	sample operator()(sample x)
    {	
		double output = 0.0;
		
		const std::vector<double>& wt = this->wavetable;

		if (in.has_signal_connection())
		{
            step_ = x / sample_rate_;
		}

		ramp_ += step_;
		while (ramp_ >= 1.0)
		{
			ramp_ -= 1.0;
		}

		output = cosip(wt.at(int(floor(ramp_ * wt.size())) % wt.size()), wt.at(int(ceil(ramp_ * wt.size())) % wt.size()), ramp_ * wt.size() - floor(ramp_ * wt.size()));

		return output;
	}

	private:
	double sample_rate_ = 44100.0;
	double frequency_ = 0.0;
	double ramp_ = 0.0;
	double step_ = 0.0;
};

MIN_EXTERNAL(nwosc_tilde);
