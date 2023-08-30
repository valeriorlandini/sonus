/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include <cstdlib>
#include <ctime>
#include "c74_min.h"
#include "blosc.h"

#define INV_RAND_MAX_2 (2.0 * (1.0 / RAND_MAX))

using namespace c74::min;

class vaosc_tilde : public object<vaosc_tilde>, public sample_operator<7, 6>
{
public:
	MIN_DESCRIPTION {"Virtual analog oscillator with classic shapes"};
	MIN_TAGS {"oscillators"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.rsosc~, saw~, tri~, cycle~, rect~, noise~"};

	inlet<>  in_f {this, "(signal/float) Frequency"};
	inlet<>  in_saw {this, "(signal/float) Saw amount (0-1)"};
	inlet<>  in_pulse {this, "(signal/float) Pulse amount (0-1)"};
	inlet<>  in_sine {this, "(signal/float) Sine amount (0-1)"};
	inlet<>  in_tri {this, "(signal/float) Triangle amount (0-1)"};
	inlet<>  in_noise {this, "(signal/float) Noise amount (0-1)"};
	inlet<>  in_pw {this, "(signal/float) Pulse width"};
	outlet<> out {this, "(signal) Output", "signal"};
	outlet<> out_saw {this, "(signal) Saw output", "signal"};
	outlet<> out_pulse {this, "(signal) Pulse output", "signal"};
	outlet<> out_sine {this, "(signal) Sine output", "signal"};
	outlet<> out_tri {this, "(signal) Triangle output", "signal"};
	outlet<> out_noise {this, "(signal) Noise output", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			srand(time(0));
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
            frequency = arg;
        }
    };
/*
	argument<number> pw_arg
	{
		this,
		"pulse width",
		"Pulse width.",
        MIN_ARGUMENT_FUNCTION
		{
            pulse_width = arg;
        }
    };
*/
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
/*
	attribute<number, threadsafe::no, limit::clamp> pulse_width
	{
        this,
        "pw",
        0.5,
		range {0.0, 1.0},
        title {"Pulse width"},
        description {"Pulse width (0-1) for the rectangular oscillator."},
		setter
		{
			MIN_FUNCTION
			{
				osc_.set_pulse_width(args[0]);
				return args;
			}
		}
    };
*/
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
				saw_amt_ = std::clamp((double)args[0], 0.0, 1.0);
				break;
				case 2:
				pulse_amt_ = std::clamp((double)args[0], 0.0, 1.0);
				break;
				case 3:
				sine_amt_ = std::clamp((double)args[0], 0.0, 1.0);
				break;
				case 4:
				tri_amt_ = std::clamp((double)args[0], 0.0, 1.0);
				break;
				case 5:
				noise_amt_ = std::clamp((double)args[0], 0.0, 1.0);
				break;
				/*case 6:
				pulse_width = args;
				break;*/
			}

			return {};
		}
    };


	samples<6> operator()(sample freq, sample saw_amt, sample pulse_amt,
	                      sample sine_amt, sample tri_amt, sample noise_amt,
						  sample pw)
    {	
		osc_.run();

		if (in_f.has_signal_connection())
			frequency = freq;

		if (in_saw.has_signal_connection())
			saw_amt_ = std::clamp((double)saw_amt, 0.0, 1.0);

		if (in_pulse.has_signal_connection())
			pulse_amt_ = std::clamp((double)pulse_amt, 0.0, 1.0);

		if (in_sine.has_signal_connection())
			sine_amt_ = std::clamp((double)sine_amt, 0.0, 1.0);

		if (in_tri.has_signal_connection())
			tri_amt_ = std::clamp((double)tri_amt, 0.0, 1.0);

		if (in_noise.has_signal_connection())
			noise_amt_ = std::clamp((double)noise_amt, 0.0, 1.0);
/*
		if (in_pw.has_signal_connection())
			pulse_width = pw;
*/
		double sine, tri, saw, pulse;
		osc_.get_last_sample(sine, tri, saw, pulse);
		double noise = ((double)rand() * INV_RAND_MAX_2) - 1.0;

		double output = sine * sine_amt_ + saw * saw_amt_ + tri * tri_amt_
		                + pulse * pulse_amt_ + noise * noise_amt_;

		if (normalize)
		{
			double sum_amt = sine_amt_ + saw_amt_ + tri_amt_ + pulse_amt_ + noise_amt_;
			if (sum_amt > 0.0)
			{
				output /= sum_amt;
			}
		}

		return {{output, saw, pulse, sine, tri, noise}};
	}

	private:
	amtl::BLOsc<double> osc_;
	double sine_amt_ = 1.0;
	double saw_amt_ = 0.0;
	double noise_amt_ = 0.0;
	double tri_amt_ = 0.0;
	double pulse_amt_ = 0.0;
};

MIN_EXTERNAL(vaosc_tilde);
