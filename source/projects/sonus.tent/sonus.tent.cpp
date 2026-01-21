/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

class tent : public object<tent>
{
public:
	MIN_DESCRIPTION {"Tent map generator"};
	MIN_TAGS {"maps, math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.logistic"};

	inlet<>  in {this, "(bang) Next value"};
	inlet<>  in_mu {this, "(float) mu parameter"};
	outlet<> out {this, "(float) Output"};

	attribute<number, threadsafe::no, limit::clamp> mu
	{
        this,
        "mu",
        1.41,
		range { 0.0, 2.0 },
        title {"mu parameter"},
		description {"mu parameter. Should be inside [0.0-2.0] range, with chaotic results achieved with values greater than 1.0."}
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
	
	argument<number> mu_arg
	{
		this,
		"mu",
		"mu parameter, from 0 to 2.",
        MIN_ARGUMENT_FUNCTION
		{
			mu = arg;
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
				mu = args;
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
			if (x < 0.5)
			{
				x *= mu;
			}
			else
			{
				x = (1.0 - x) * mu;
			}
			out.send(x.get());
			return {};
		}
	};
};

MIN_EXTERNAL(tent);
