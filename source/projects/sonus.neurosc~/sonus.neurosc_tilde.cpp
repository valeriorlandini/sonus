/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "biquad.h"
#include "neurowave.h"
#include "wtosc.h"

using namespace c74::min;
using namespace soutel;

class neurosc_tilde : public object<neurosc_tilde>, public sample_operator<9, 1>
{
public:
	MIN_DESCRIPTION {"Wavetable oscillator with user defined table"};
	MIN_TAGS {"oscillators"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.ecaosc~, sonus.nwosc~"};

	inlet<>  in_f {this, "(signal/float) Frequency"};
	inlet<>  in_l1 {this, "(signal/float) Latent parameter 1"};
	inlet<>  in_l2 {this, "(signal/float) Latent parameter 2"};
	inlet<>  in_l3 {this, "(signal/float) Latent parameter 3"};
	inlet<>  in_l4 {this, "(signal/float) Latent parameter 4"};
	inlet<>  in_l5 {this, "(signal/float) Latent parameter 5"};
	inlet<>  in_l6 {this, "(signal/float) Latent parameter 6"};
	inlet<>  in_l7 {this, "(signal/float) Latent parameter 7"};
	inlet<>  in_l8 {this, "(signal/float) Latent parameter 8"};
	outlet<> out {this, "(signal) Output", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			oscillator_.set_sample_rate(double(args[0]));
			interval_ = (int)args[0] / 20;
			elapsed_ = 0;
			waveform_.set_latent_space(latent_);
			waveform_.generate_wave();
			oscillator_.set_wavetable(waveform_.get_wavetable());
			lowpass_.set_sample_rate(double(args[0]));
			lowpass_.set_cutoff(double(args[0]) * 0.375);
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
				oscillator_.set_frequency((double)args[0]);
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
			if (inlet == 0)
			{
				frequency = args;
			}
			else
			{
				latent_.at(inlet - 1) = double(args[0]);
				changed_ = true;
			}
			return {};
		}
    };

	sample operator()(sample freq, sample l1, sample l2, sample l3, sample l4, sample l5, sample l6, sample l7, sample l8)
    {	
		double output = 0.0;

		if (in_f.has_signal_connection())
			frequency = freq;

		if (in_l1.has_signal_connection()) {
		    if ((double)l1 != latent_.at(0))
			{
		        latent_.at(0) = (double)l1;
		        changed_ = true;
		    }
		}

		if (in_l2.has_signal_connection()) {
		    if ((double)l2 != latent_.at(1))
			{
		        latent_.at(1) = (double)l2;
		        changed_ = true;
		    }
		}

		if (in_l3.has_signal_connection()) {
		    if ((double)l3 != latent_.at(2))
			{
		        latent_.at(2) = (double)l3;
		        changed_ = true;
		    }
		}

		if (in_l4.has_signal_connection()) {
		    if ((double)l4 != latent_.at(3))
			{
		        latent_.at(3) = (double)l4;
		        changed_ = true;
		    }
		}

		if (in_l5.has_signal_connection()) {
		    if ((double)l5 != latent_.at(4))
			{
		        latent_.at(4) = (double)l5;
		        changed_ = true;
		    }
		}

		if (in_l6.has_signal_connection()) {
		    if ((double)l6 != latent_.at(5))
			{
		        latent_.at(5) = (double)l6;
		        changed_ = true;
		    }
		}

		if (in_l7.has_signal_connection()) {
		    if ((double)l7 != latent_.at(6))
			{
		        latent_.at(6) = (double)l7;
		        changed_ = true;
		    }
		}

		if (in_l8.has_signal_connection()) {
		    if ((double)l8 != latent_.at(7))
			{
		        latent_.at(7) = (double)l8;
		        changed_ = true;
		    }
		}

		if (++elapsed_ >= interval_)
		{
			elapsed_ = 0;
			if (changed_)
			{
				
				std::vector<double> wt = oscillator_.get_wavetable();
				waveform_.set_latent_space(latent_);
				waveform_.generate_wave();
				oscillator_.set_wavetable(waveform_.get_wavetable());
				changed_ = false;
			}
		}

		return lowpass_.run(oscillator_.run() * 0.707);
	}

	private:
	WTOsc<double> oscillator_;
	Neurowave<double> waveform_;
	std::array<double, 8> latent_ = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	Biquad<double> lowpass_;
	int elapsed_;
	int interval_;
	bool changed_ = false;
};

MIN_EXTERNAL(neurosc_tilde);
