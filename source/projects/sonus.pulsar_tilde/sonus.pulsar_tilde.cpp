/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "pulsar.h"

using namespace c74::min;
using namespace soutel;

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

	enum class waveforms : int { sine, triangle, saw, square, noise, dc, phasor, enum_count };
    enum_map waveforms_range = {"sine", "triangle", "saw", "square", "noise", "dc", "phasor"};

	enum class windows : int { hann, hamming, blackman, nuttall, blackmannuttall, blackmanharris, flattop, bartletthann, rectangular, enum_count };
    enum_map windows_range = {"hann", "hamming", "blackman", "nuttall", "blackman-nuttall", "blackman-harris", "flat-top", "bartlett-hann", "rectangular"};

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
            pulsar_.set_frequency(double(arg));
        }
    };

	argument<number> duty_cycle_arg
	{
		this,
		"duty cycle",
		"Duty cycle (0-1).",
        MIN_ARGUMENT_FUNCTION
		{
			pulsar_.set_duty_cycle(double(arg));
        }
    };

    attribute<windows> window
	{
		this,
		"window",
		windows::rectangular,
		windows_range,
		title {"Window"},
        description {"Window function to multiply the pulsar."},
		setter
		{
			MIN_FUNCTION
			{
				pulsar_.set_window((PulsarWindows)args[0]);
				return args;
			}
		}
    };

    attribute<waveforms> waveform
	{
		this,
		"waveform",
		waveforms::sine,
		waveforms_range,
		title {"Waveform"},
        description {"Waveform of the pulsar."},
		setter
		{
			MIN_FUNCTION
			{
				pulsar_.set_waveform((PulsarWaveforms)args[0]);
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
				pulsar_.set_frequency(double(args[0]));
				break;
				case 1:
				pulsar_.set_duty_cycle(double(args[0]));
				break;
			}

			return {};
		}
    };

	sample operator()(sample freq, sample dc)
    {	
		if (in_f.has_signal_connection())
		{
			pulsar_.set_frequency(freq);
		}

		if (in_dc.has_signal_connection())
		{
			pulsar_.set_duty_cycle(dc);
		}

		return pulsar_.run();
	}

	private:
	Pulsar<double> pulsar_;
};

MIN_EXTERNAL(pulsar_tilde);
