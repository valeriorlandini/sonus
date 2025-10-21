/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

class logistic : public object<logistic>
{
public:
	MIN_DESCRIPTION {"Logistic map generator"};
	MIN_TAGS {"maps, math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.tent"};

	inlet<>  in {this, "(bang) Next value"};
	inlet<>  in_mu {this, "(float) r parameter"};
	outlet<> out {this, "(float) Output"};

	attribute<number, threadsafe::no, limit::clamp> r
	{
        this,
        "r",
        3.6,
		range { 0.0, 3.999999 },
        title {"r parameter"},
		description {"r parameter. Should be inside [0.0-4.0) range, with divergent trajectories with r > 3.0 and chaotic one with r > 3.56995."}
    };

	attribute<number, threadsafe::no, limit::clamp> x
	{
        this,
        "x",
        0.5,
		range { 0.0, 1.0 },
        title {"Initial value"},
		description {"Initial value. Should be inside [0.0-1.0] range."}
    };
	
	argument<number> r_arg
	{
		this,
		"r",
		"r parameter, from 0 to 4.",
        MIN_ARGUMENT_FUNCTION
		{
			r = arg;
        }
    };
	
	message<> m_number
	{
		this,
		"number",
		"Depends on inlet. A number on the first inlet resets the initial value, while on the second inlet sets the mu parameter.",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				x = args;
			}
			else
			{
				r = args;
			}

			return {};
		}
    };

	message<> m_bang
	{
		this,
		"bang",
		"Output next value",
		MIN_FUNCTION
		{
			x = r * x * (1.0 - x);
			out.send(x.get());
			return {};
		}
	};
};

MIN_EXTERNAL(logistic);
