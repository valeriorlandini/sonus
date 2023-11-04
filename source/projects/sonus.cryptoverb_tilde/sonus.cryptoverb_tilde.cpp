/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../include/cryptoverb.h"

using namespace c74::min;

class cryptoverb_tilde : public object<cryptoverb_tilde>, public sample_operator<2, 2>
{
public:
	MIN_DESCRIPTION {"Multistage (sort of) reverb"};
	MIN_TAGS {"effects, reverb"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {""};

	inlet<>  in_l {this, "(signal) Left channel input"};
	inlet<>  in_r {this, "(signal) Right channel input"};
	inlet<>  in_w1 {this, "(float) Block 1 wet"};
	inlet<>  in_w2 {this, "(float) Block 2 wet"};
	inlet<>  in_w3 {this, "(float) Block 3 wet"};
	inlet<>  in_w4 {this, "(float) Block 4 wet"};
	outlet<> out_l {this, "(signal) Left channel output", "signal"};
	outlet<> out_r {this, "(signal) Right channel output", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			cryptoverb_.set_sample_rate(args[0]);
			return {};
		}
	};

	message<> m_number
	{
		this,
		"number",
		"Block wet",
        MIN_FUNCTION
		{
			if (inlet >= 2)
			{
				cryptoverb_.set_block_wet(args[0], inlet - 1);
			}

			return {};
		}
    };

	samples<2> operator()(sample input_l, sample input_r)
    {
		std::array<double, 2> outputs = cryptoverb_.run(input_l, input_r);

		return { {outputs[0], outputs[1]} };
	}
private:
	Cryptoverb<double> cryptoverb_;
};

MIN_EXTERNAL(cryptoverb_tilde);
