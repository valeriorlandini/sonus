/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <vector>
#include "interp.h"

using namespace c74::min;
using namespace soutel;

class collatz_tilde : public object<collatz_tilde>, public sample_operator<1, 1>
{
private:
	std::vector<sample> wavetable_ = {0.0};
public:
	MIN_DESCRIPTION {"Oscillator with wavetable created from Collatz conjecture rules"};
	MIN_TAGS {"oscillator, noise"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.nowave~, sonus.nwosc~"};

	inlet<>  in {this, "(signal) Wavetable Position (from 0 to 1)"};
	inlet<>  in_s {this, "(int) Seed"};
	outlet<> out {this, "(signal) Output", "signal"};

	argument<int> start_arg
	{
		this,
		"start",
		"Starting number (2-1000).",
		MIN_ARGUMENT_FUNCTION
		{
            start = arg;
        }
    };

    attribute<int, threadsafe::no, limit::clamp> start
	{
		this,
		"start",
		42,
		range {2, 1000},
		title {"Start"},
        description {"Start number for wavetable generation."},
		setter
		{
			MIN_FUNCTION
			{
				wt_gen(int(args[0]));
				return args;
			}
		}
    };

	attribute<bool> interp
	{
        this,
        "interp",
        true,
        title {"Interpolate values"},
        description {"If active, the values of the wavetable are interpolated."}
    };

	sample operator()(sample x)
    {	
		if (wavetable_.size() == 0)
		{
			return 0.0;
		}

		sample scaled = abs(x) * static_cast<sample>(wavetable_.size());
		unsigned int a = (unsigned int)floor(scaled) % wavetable_.size();

		sample output, dummy;

		if (interp)
		{
			unsigned int b = (unsigned int)ceil(scaled) % wavetable_.size();
			output = cosip(wavetable_[a], wavetable_[b], std::modf(scaled, &dummy));
		}
		else
		{
			output = wavetable_[a];		
		}

		return output;
	}
		
	private:
	inline void wt_gen(const int &start)
	{
		wavetable_.clear();
		int value = start;
		int max = start;
		
		wavetable_.push_back(static_cast<sample>(value));

		do
		{
			if (value % 2)
			{
				value *= 3;
				value++;
			}
			else
			{
				value /= 2;			
			}

			if (value > max)
			{
				max = value;
			}
			
			wavetable_.push_back(static_cast<sample>(value));
		}
		while (value != 1);

		sample max_value = static_cast<sample>(max);

		for (auto s = 0; s < wavetable_.size(); s++)
		{
			wavetable_[s] /= max_value;
		}
	}
};

MIN_EXTERNAL(collatz_tilde);
