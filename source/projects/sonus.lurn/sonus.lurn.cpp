/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

class lurn : public object<lurn>
{
public:
	MIN_DESCRIPTION {"Non repeating random sequence of random symbols"};
	MIN_TAGS {"random"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"urn, random"};

	inlet<>  in {this, "(bang) Next element"};
	inlet<>  in_l {this, "(list) Elements to choose from"};
	outlet<> out {this, "(symbol) Output"};
	outlet<> out_end {this, "(bang) Bang when all elements have been chosen"};

	
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

	private:
	std::vector<std::string> elements_;
	std::vector<std::string> elements_out_;
};

MIN_EXTERNAL(lurn);
