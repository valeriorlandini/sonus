/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "addosc.h"

using namespace c74::min;
using namespace soutel;

class harmosc_tilde : public object<harmosc_tilde>, public vector_operator<>
{
private:
	AddOsc<double> osc_;
	int harmonics_;
	std::vector<std::unique_ptr<inlet<>>> m_inlets;
	
public:
	MIN_DESCRIPTION {"Additive harmonic oscillator"};
	MIN_TAGS {"oscillators"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.blosc~, sonus.nwosc~, sonus.rsosc~, cycle~"};

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

    harmosc_tilde(const atoms& args = {})
    {
        harmonics_ = 8;

		if (!args.empty())
		{
            harmonics_ = std::min(std::max(int(args[0]), 1), 32);
		}

        if (args.size() > 1)
        {
			osc_.set_frequency(double(args[1]));
        }

		osc_.set_harmonics(harmonics_);

	    for (auto i = 0; i < harmonics_ + 1; ++i)
        {
            if (i == 0)
            {
                m_inlets.push_back(std::make_unique<inlet<>>(this, "(signal/float) Frequency", "signal"));
            }
            else
            {
                m_inlets.push_back(std::make_unique<inlet<>>(this, "(signal/float) Harmonic " + std::to_string(i) + " gain", "signal"));
            }
        }
		
    }

	argument<int> harmonics_arg
	{
		this,
		"harmonics",
		"Number of harmonics (1-32)."
    };

	argument<number> frequency_arg
	{
		this,
		"frequency",
		"Oscillator frequency."
    };

    attribute<bool, threadsafe::no> normalize
	{
        this,
        "normalize",
        true,
        title {"Normalize gains"},
        description {"If enabled, gains are normalized so that output does not clip."},
		setter
		{
			MIN_FUNCTION
			{
				osc_.set_normalize(bool(args[0]));
				
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
				osc_.set_frequency(double(args[0]));
			}
			else
			{
				osc_.set_harmonic_gain(inlet - 1, double(args[0]));
			}

			return {};
		}
    };

	message<> gains
	{
		this,
		"gains",
		"Sets all the gains.",
        MIN_FUNCTION
		{
			for (auto g = 0; g < std::min(harmonics_, int(args.size())); g++)
			{
				osc_.set_harmonic_gain(g, double(args[g]));
			}
			return {};
		}
    };

	message<> phases
	{
		this,
		"phases",
		"Sets all the phases.",
        MIN_FUNCTION
		{
			for (auto p = 0; p < std::min(harmonics_, int(args.size())); p++)
			{
				osc_.set_harmonic_phase(p, double(args[p]));
			}
			return {};
		}
    };	

	message<> gain
	{
		this,
		"gain",
		"Set the gain of the specified harmonic.",
        MIN_FUNCTION
		{
			if (args.size() >= 2)
			{
				osc_.set_harmonic_gain(int(args[0]) - 1, double(args[1]));
			}
			return {};
		}
    };

	message<> phase
	{
		this,
		"phase",
		"Set the phase of the specified harmonic.",
        MIN_FUNCTION
		{
			if (args.size() >= 2)
			{
				osc_.set_harmonic_phase(int(args[0]) - 1, double(args[1]));
			}
			return {};
		}
    };	

	void operator()(audio_bundle input, audio_bundle output)
    {	
		for (auto i = 0; i < input.frame_count(); ++i)
        {
			if (m_inlets.at(0)->has_signal_connection())
			{
           		osc_.set_frequency((double)input.samples(0)[i]);
			}

            for (auto ch = 1; ch <= harmonics_; ch++)
            {
				if (m_inlets.at(ch)->has_signal_connection())
				{
           			osc_.set_harmonic_gain(ch - 1, (double)input.samples(ch)[i]);
				}
            }

			output.samples(0)[i] = osc_.run();
	    }
	}
};

MIN_EXTERNAL(harmosc_tilde);
