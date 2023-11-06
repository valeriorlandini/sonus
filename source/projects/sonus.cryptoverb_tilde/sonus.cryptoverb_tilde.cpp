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

	inlet<> in_l {this, "(signal) Left channel input"};
	inlet<> in_r {this, "(signal) Right channel input"};
	inlet<> in_lp {this, "(float) Master lowpass frequency"};
	inlet<> in_gw {this, "(float) Global wet (0-1)"};
	inlet<> in_w1 {this, "(float) Block 1 wet (0-1)"};
	inlet<> in_w2 {this, "(float) Block 2 wet (0-1)"};
	inlet<> in_w3 {this, "(float) Block 3 wet (0-1)"};
	inlet<> in_w4 {this, "(float) Block 4 wet (0-1)"};
	inlet<> in_pm {this, "(int) Serial or parallel mode (0/1)"};
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

	attribute<number, threadsafe::no, limit::clamp> wet
	{
        this,
        "wet",
        0,
		range { 0.0, 1.0 },
        title {"Global wet"}
    };

	message<> m_number
	{
		this,
		"number",
		"Block wet",
        MIN_FUNCTION
		{
			switch (inlet)
			{
				case 2:
				cryptoverb_.set_lowpass_cutoff(args[0]);
				break;
				case 3:
				wet = args[0];
				break;
				case 4:
				case 5:
				case 6:
				case 7:
				cryptoverb_.set_block_wet(args[0], inlet - 3);
				break;
				case 8:
				cryptoverb_.set_mode(bool(args[0]));
				break;
			}

			return {};
		}
    };

	samples<2> operator()(sample input_l, sample input_r)
    {
		std::array<double, 2> outputs = cryptoverb_.run(input_l, input_r);

		return { {(outputs[0] * wet) + ((1.0 - wet) * input_l), (outputs[1] * wet) + ((1.0 - wet) * input_r)} };
	}
private:
	Cryptoverb<double> cryptoverb_;
};

MIN_EXTERNAL(cryptoverb_tilde);
