/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../include/chebyshev.h"
#include "../include/interp.h"
#include <algorithm>

using namespace c74::min;


class cheby_tilde : public object<cheby_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Shape incoming signal through Chebyshev polynomials"};
	MIN_TAGS {"effects, waveshaper"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"lookup~"};

	inlet<>  in {this, "(signal) Input"};
	inlet<>  in_o {this, "(int) Order of the polynomial (0-10)"};
	outlet<> out {this, "(signal) Output", "signal"};

	argument<number> order_arg
	{
		this,
		"order",
		"Polynomial order (0-10).",
        MIN_ARGUMENT_FUNCTION
		{
            p_order = arg;
        }
    };

	attribute<number, threadsafe::no, limit::clamp> p_order
	{
        this,
        "order",
        1.0,
		range { 0.0, 10.0 },
        title {"Order"},
        description {"Order of the Chebyshev polynomial to use. Orders up to 10 are supported, non integer number interpolates the two nearest polynomials."},
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
		"Polynomial order (0-10)",
        MIN_FUNCTION
		{
			if (inlet == 1)
			{
				p_order = args;
			}

			return {};
		}
    };

	sample operator()(sample input)
    {
		sample output = cosip(chebyshev(input, (unsigned int)std::floor(p_order)),
		                      chebyshev(input, (unsigned int)std::ceil(p_order)),
							  p_order - std::floor(p_order));
		return { output };
	}
};

MIN_EXTERNAL(cheby_tilde);
