/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <chrono>
#include <random>
#include <vector>
#include "interp.h"

using namespace c74::min;
using namespace soutel;

class nowave_tilde : public object<nowave_tilde>, public sample_operator<1, 1>
{
private:
	std::vector<number> wavetable_ = {0.0};
	std::vector<number> values_ = {0.0};
public:
	MIN_DESCRIPTION {"Random wavetable oscillator with recallable seeds"};
	MIN_TAGS {"oscillator, noise"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"noise~, rand~, sonus.nwosc~"};

	inlet<>  in {this, "(signal) Wavetable Position (from 0 to 1)"};
	inlet<>  in_s {this, "(int) Seed"};
	outlet<> out {this, "(signal) Output", "signal"};

	argument<int> size_arg
	{
		this,
		"size",
		"Wavetable size (2-16384)."
    };

	argument<int> seed_arg
	{
		this,
		"seed",
		"Seed for wavetable generation."
    };

    attribute<int> seed
	{
		this,
		"seed",
		0,
		range {0, 4294967296},
		title {"Seed"},
        description {"Seed for wavetable generation."},
		setter
		{
			MIN_FUNCTION
			{
				if (values_.size() > 1)
				{
					shuffle_wavetable(args[0]);
				}
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

	nowave_tilde(const atoms& args = {})
    {
		wt_gen(256);
		int wt_size = 256;

		if (!args.empty())
		{
			wt_size = int(args[0]);
			if (wt_size < 2)
			{
				wt_size = 2;
			}
			if (wt_size > 16384)
			{
				wt_size = 16384;
			}
		}

		wt_gen(wt_size);

		if (args.size() > 1)
		{
			seed = int(args[1]);
		}
		else
		{
			seed = int(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());	
		}
    }

    message<> m_number
	{
		this,
		"number",
		"In second inlet, seed",
        MIN_FUNCTION
		{
			if (inlet == 1)
            {
                seed = int(args[0]);
            }
			return {};
		}
    };

	sample operator()(sample x)
    {	
		if (wavetable_.size() == 0)
		{
			return 0.0;
		}

		sample scaled = abs(x) * (sample)wavetable_.size();
		unsigned int a = (unsigned int)floor(scaled) % wavetable_.size();

		sample output, dummy;

		if (interp)
		{
			unsigned int b = (unsigned int)ceil(scaled) % wavetable_.size();
			output = cosip(wavetable_.at(a), wavetable_.at(b), std::modf(scaled, &dummy));
		}
		else
		{
			output = wavetable_.at(a);		
		}

		return output;
	}
		
	private:
	inline void wt_gen(const int &size)
	{
    	number step = 1.0 / ((number)size - 1.0);
		values_.resize(size);
    
		for (auto i = 0; i < size; i++)
		{
        	values_.at(i) = (number)i * step;
    	}
	}

	inline void shuffle_wavetable(int seed)
	{
    	std::mt19937 g;

    	if (seed < 0)
		{
        	std::random_device rd;
        	g.seed(rd());
    	}
		else
		{
        	g.seed(static_cast<unsigned>(seed));
    	}
    	
		wavetable_ = values_;

		if (wavetable_.size() > 0)
		{
			std::shuffle(wavetable_.begin(), wavetable_.end(), g);
		}
	}
};

MIN_EXTERNAL(nowave_tilde);
