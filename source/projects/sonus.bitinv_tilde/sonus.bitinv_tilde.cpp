/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2025 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;

class bitinv_tilde : public object<bitinv_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Invert the bits of an 8-bit bitreduced signal"};
	MIN_TAGS {"effects, glitch, bitcrusher"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.bitman~"};

	inlet<>  in {this, "(signal) Input"};
	inlet<>  in_1 {this, "(int) Invert 1st bit (0-1)"};
	inlet<>  in_2 {this, "(int) Invert 2nd bit (0-1)"};
	inlet<>  in_3 {this, "(int) Invert 3rd bit (0-1)"};
	inlet<>  in_4 {this, "(int) Invert 4th bit (0-1)"};
	inlet<>  in_5 {this, "(int) Invert 5th bit (0-1)"};
	inlet<>  in_6 {this, "(int) Invert 6th bit (0-1)"};
	inlet<>  in_7 {this, "(int) Invert 7th bit (0-1)"};
	inlet<>  in_8 {this, "(int) Invert 8th bit (0-1)"};
	outlet<> out {this, "(signal) Output", "signal"};

	message<> m_number
	{
		this,
		"number",
		"Invert n-th bit",
        MIN_FUNCTION
		{
			bit_status_[inlet - 1] = (bool)args[0];
			return {};
		}
    };

	message<> set
	{
		this,
		"set",
		"On first inlet, set the bit status.",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				for (auto i = 0; i < args.size(); i++)
				{
					if (i < 8)
					{
						bit_status_[i] = (bool)args[i];
					}
				}
			}

			return {};
		}
    };

	sample operator()(sample input)
    {
        unsigned char in = (unsigned char)round((1.0 + std::clamp(input, -1.0, 1.0)) * 0.5 * 255.0);

    	for (int b = 0; b < 8; b++)
    	{
        	if (bit_status_[7 - b])
        	{
				in ^= 1 << b;
        	}
    	}

    	sample out = (((double)in / 255.0) * 2.0) - 1.0;

		return { out };
	}

	private:
	bool bit_status_[8] = {false, false, false, false, false, false, false, false};
};

MIN_EXTERNAL(bitinv_tilde);
