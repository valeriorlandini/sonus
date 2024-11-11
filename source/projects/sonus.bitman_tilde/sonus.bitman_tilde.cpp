/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>

using namespace c74::min;

class bitman_tilde : public object<bitman_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Manipulate the single bits of a signal"};
	MIN_TAGS {"effects, glitch, bitcrusher"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.bitinv~"};

	inlet<>  in {this, "(signal) Input"};
	outlet<> out {this, "(signal) Output", "signal"};

	bitman_tilde(const atoms& args = {})
    {
        bits_ = 8;

        if (!args.empty() && int(args[0]) > 0)
        {
            bits_ = std::min(int(args[0]), 32);
        }

		bit_op_.clear();

		for (auto b = 0; b < bits_; b++)
		{
			bit_op_.push_back(BitOp::None);
		}
    }

	message<> set
	{
		this,
		"set",
		"Set the bit operation (0: none, 1: toggle, 2: set, 3: clear).",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				for (auto i = 0; i < args.size(); i++)
				{
					if (i < bit_op_.size())
					{
						bit_op_.at(i) = (BitOp)std::clamp(int(args[i]), 0, 3);
					}
				}
			}

			return {};
		}
    };

	sample operator()(sample input)
    {
        unsigned long in = (unsigned long)round((1.0 + std::clamp(input, -1.0, 1.0)) * 0.5 * (pow(2.0, (double)bits_) - 1.0));

    	for (int b = 0; b < bits_; b++)
    	{
        	switch (bit_op_[bits_ - b - 1])
        	{
				case BitOp::Toggle:
				in ^= 1 << b;
				break;
				case BitOp::Set:
				in |= 1 << b;
				break;
				case BitOp::Clear:
				in &= ~(1 << b);
				break;
        	}
    	}

    	sample out = (((double)in / (pow(2.0, (double)bits_) - 1.0)) * 2.0) - 1.0;

		return { out };
	}

	private:
	enum class BitOp
	{
		None,
		Toggle,
		Set,
		Clear
	};
	unsigned short bits_;
	std::vector<BitOp> bit_op_;
};

MIN_EXTERNAL(bitman_tilde);
