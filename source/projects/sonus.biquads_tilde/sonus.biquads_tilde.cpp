/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "biquad.h"

using namespace c74::min;
using namespace soutel;

class biquads_tilde : public object<biquads_tilde>, public sample_operator<4, 7>
{
public:
	MIN_DESCRIPTION {"Parallel biquad filters"};
	MIN_TAGS {"effects, filter"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"biquad~, lores~, reson~, svf~"};

	inlet<>  in {this, "(signal) Input"};
	inlet<>  in_c {this, "(signal/float) Cutoff"};
	inlet<>  in_q {this, "(signal/float) Q factor"};
	inlet<>  in_g {this, "(signal/float) Shelving and peak gain"};
	outlet<> out_lp {this, "(signal) Lowpass output", "signal"};
	outlet<> out_hp {this, "(signal) Highpass output", "signal"};
	outlet<> out_bp {this, "(signal) Bandpass output", "signal"};
	outlet<> out_br {this, "(signal) Bandreject output", "signal"};
	outlet<> out_ls {this, "(signal) Low shelf output", "signal"};
	outlet<> out_hs {this, "(signal) High shelf output", "signal"};
	outlet<> out_pk {this, "(signal) Peak output", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			lowpass_.set_sample_rate(args[0]);
			highpass_.set_sample_rate(args[0]);
			bandpass_.set_sample_rate(args[0]);
			bandreject_.set_sample_rate(args[0]);
			lowshelf_.set_sample_rate(args[0]);
			highshelf_.set_sample_rate(args[0]);
			peak_.set_sample_rate(args[0]);
			return {};
		}
	};

	argument<number> cutoff_arg
	{
		this,
		"cutoff",
		"Cutoff frequency in Hz.",
        MIN_ARGUMENT_FUNCTION
		{
			set_cutoffs(double(arg));
        }
    };

	argument<number> q_arg
	{
		this,
		"q",
		"Q factor (filter resonance).",
        MIN_ARGUMENT_FUNCTION
		{
			set_qs(double(arg));
        }
    };

	argument<number> gain_arg
	{
		this,
		"gain",
		"Gain in dB for shelving and peak filters.",
        MIN_ARGUMENT_FUNCTION
		{
            set_gains(double(arg));
        }
    };

	message<> m_number
	{
		this,
		"number",
		"Depends on the inlet",
        MIN_FUNCTION
		{
			if (inlet == 1)
			{
				set_cutoffs(double(args[0]));
			}
			if (inlet == 2)
			{	
				set_qs(double(args[0]));
			}
			if (inlet == 3)
			{
				set_gains(double(args[0]));
			}

			return {};
		}
    };

	message<> clear
    {
        this,
        "clear",
        "Clear the memory of the filters",
        MIN_FUNCTION
        {
            lowpass_.clear();
            highpass_.clear();
            bandpass_.clear();
            bandreject_.clear();
            lowshelf_.clear();
            highshelf_.clear();
            peak_.clear();

			return {};
		}
	};

	samples<7> operator()(sample input, sample sig_cutoff, sample sig_q, sample sig_gain)
    {
		if (in_c.has_signal_connection())
		{
			set_cutoffs(sig_cutoff);
		}

		if (in_q.has_signal_connection())
		{
			set_qs(sig_q);
		}

		if (in_g.has_signal_connection())
		{
			set_gains(sig_gain);
		}

		sample lp = lowpass_.run(input);
		sample hp = highpass_.run(input);
		sample bp = bandpass_.run(input);
		sample br = bandreject_.run(input);
		sample ls = lowshelf_.run(input);
		sample hs = highshelf_.run(input);
		sample pk = peak_.run(input);
	
		return { {lp, hp, bp, br, ls, hs, pk} };
	}

	private:
	Biquad<double> lowpass_{Biquad<double>(44100.0, 20000.0, 0.707, 0.0, BQFilters::lowpass)};
	Biquad<double> highpass_{Biquad<double>(44100.0, 20000.0, 0.707, 0.0, BQFilters::hipass)};
	Biquad<double> bandpass_{Biquad<double>(44100.0, 20000.0, 0.707, 0.0, BQFilters::bandpass)};
	Biquad<double> bandreject_{Biquad<double>(44100.0, 20000.0, 0.707, 0.0, BQFilters::bandreject)};
	Biquad<double> lowshelf_{Biquad<double>(44100.0, 20000.0, 0.707, 0.0, BQFilters::lowshelf)};
	Biquad<double> highshelf_{Biquad<double>(44100.0, 20000.0, 0.707, 0.0, BQFilters::hishelf)};
	Biquad<double> peak_{Biquad<double>(44100.0, 20000.0, 0.707, 0.0, BQFilters::peak)};

	inline void set_cutoffs(const double &cutoff_value)
	{
		double cutoff = std::clamp(cutoff_value, 0.0, samplerate() * 0.5);

		lowpass_.set_cutoff(cutoff);
		highpass_.set_cutoff(cutoff);
		bandpass_.set_cutoff(cutoff);
		bandreject_.set_cutoff(cutoff);
		lowshelf_.set_cutoff(cutoff);
		highshelf_.set_cutoff(cutoff);
		peak_.set_cutoff(cutoff);
	}

	inline void set_qs(const double &q_value)
	{
		double q = std::clamp(q_value, 0.0, 100.0);
		
		lowpass_.set_q(q);
		highpass_.set_q(q);
		bandpass_.set_q(q);
		bandreject_.set_q(q);
		lowshelf_.set_q(q);
		highshelf_.set_q(q);
		peak_.set_q(q);
	}

	inline void set_gains(const double &gain_value)
	{
		double gain = std::clamp(gain_value, -48.0, 48.0);

		lowpass_.set_gain(gain);
		highpass_.set_gain(gain);
		bandpass_.set_gain(gain);
		bandreject_.set_gain(gain);
		lowshelf_.set_gain(gain);
		highshelf_.set_gain(gain);
		peak_.set_gain(gain);
	}
};

MIN_EXTERNAL(biquads_tilde);
