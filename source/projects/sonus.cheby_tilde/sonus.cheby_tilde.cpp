/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
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

	argument<int> order_arg
	{
		this,
		"order",
		"Polynomial order (0-10).",
        MIN_ARGUMENT_FUNCTION
		{
            p_order = arg;
        }
    };

	attribute<int, threadsafe::no, limit::clamp> p_order
	{
        this,
        "order",
        1,
		range { 0, 10 },
        title {"Order"},
        description {"Order of the Chebyshev polynomial to use. Orders up to 10 are supported."},
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
		"int",
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
		sample output;

		switch (p_order)
    	{
    		case 0:
    		    output = 1.0;
    		    break;
    		case 1:
    		    output = input;
    		    break;
    		case 2:
    		    output = 2.0 * pow(input, 2.0) - 1.0;
    		    break;
    		case 3:
    		    output = 4.0 * pow(input, 3.0) - 3.0 * input;
    		    break;
    		case 4:
    		    output = 8.0 * pow(input, 4.0) - 8.0 * pow(input, 2.0) + 1.0;
    		    break;
    		case 5:
    		    output = 16.0 * pow(input, 5.0) - 20.0 * pow(input, 3.0) + 5.0 * input;
    		    break;
    		case 6:
    		    output = 32.0 * pow(input, 6.0) - 48.0 * pow(input, 4.0) + 18.0 * pow(input, 2.0) - 1.0;
    		    break;
    		case 7:
    		    output = 64.0 * pow(input, 7.0) - 112.0 * pow(input, 5.0) + 56.0 * pow(input, 3.0) - 7.0 * input;
    		    break;
    		case 8:
    		    output = 128.0 * pow(input, 8.0) - 256.0 * pow(input, 6.0) + 160.0 * pow(input, 4.0) - 32.0 * pow(input, 2.0) + 1.0;
    		    break;
    		case 9:
    		    output = 256.0 * pow(input, 9.0) - 576.0 * pow(input, 7.0) + 432.0 * pow(input, 5.0) - 120.0 * pow(input, 3.0) + 9.0 * input;
    		    break;
    		case 10:
    		    output = 512.0 * pow(input, 10.0) - 1280.0 * pow(input, 8.0) + 1120.0 * pow(input, 6.0) - 400.0 * pow(input, 4.0) + 50.0 * pow(input, 2.0) - 1.0;
    		    break;
    	}

		return { output };
	}
};

MIN_EXTERNAL(cheby_tilde);
