/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cstdint>

using namespace c74::min;

class byteplay_tilde : public object<byteplay_tilde>, public sample_operator<0, 1>
{
public:
	MIN_DESCRIPTION {"Bytebeat music player"};
	MIN_TAGS {"algorithmic, player"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.bytebeat~"};

	inlet<>  in_f {this, "(int) Formula preset index (0-15)"};
	inlet<>  in_sr {this, "(int) Sample rate reduction factor (1-10)"};
	outlet<> out {this, "(signal) Output", "signal"};

	enum class formulas : int { zero, one, two, three, four, five, six, seven,
	                            eight, nine, ten, eleven, twelve, thirteen,
								fourteen, fifteen, enum_count };

	enum_map formulas_range = {"t", "t*5&t>>7", "(t*9&t>>4)|(t*5&t>>7)|(t*3&t>>10))-1",
	                           "(t>>8&t)*(t>>15&t)", "(t%(t>>8|t>>16))^t",
							   "(t%255&t)-(t>>13&t)", "(t-(t>>4&t>>8)&t>>12)-1",
							   "((t*t)/(t^t>>8))&t", "((2*(t&1)-1)*t)-(t>>8)",
							   "t*(t>>(t>>13&t))", "(t*t/(1+(t>>9&t>>8)))&128",
							   "(t*(-(t>>8|t|t>>9|t>>13)))^t", "(t<<13)|(t&t>>3)*(t>>11&t)-(t>>(t^17))-2",
							   "(t&t>>4)-(t>>13&t)", "((t/1000)^(t/1001))*t",
							   "(((t&t>>8)-(t>>13&t))&((t&t>>8)-(t>>13)))^(t>>8&t)"};

	attribute<formulas> formula
	{
        this,
        "formula",
        formulas::one,
		formulas_range,
        title {"Formula"},
		description {"Formula to use."},
		setter
		{
			MIN_FUNCTION
			{
                formula_idx_ = int(args[0]);
				return args;
			}
		}
    };

	attribute<int, threadsafe::no, limit::clamp> sr_red
	{
        this,
        "red",
        4,
		range { 1, 10 },
        title {"SR Reduction"},
        description {"Factor for sample rate reduction. The lower it is, the higher the pitch of the sound is."},
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
		"Depends on inlet",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				formula = formulas(std::clamp(int(args[0]), 0, int(formulas::enum_count) - 1));
			}

			if (inlet == 1)
			{
				sr_red = args[0];
			}

			return {};
		}
    };

	message<> reset
    {
        this,
        "reset",
        "Reset t variable to 0",
        MIN_FUNCTION
        {
            t_ = 0;

			return {};
		}
	};

	sample operator()()
    {
		++sample_count_;

		if (sample_count_ >= sr_red)
		{
			++t_;
			uint8_t out = 0;

			switch (formula_idx_)
			{
				case 0:
				out = t_;
				break;
				case 1:
				out = t_*5 & t_>>7;
				break;
				case 2:
				out = (t_*9 & t_>>4 | t_*5 & t_>>7 | t_*3 & t_>>10) - 1;
				break;
				case 3:
				out = (t_>>8 & t_) * (t_>>15 & t_);
				break;
				case 4:
				out = (t_%(t_>>8|t_>>16))^t_;
				break;
				case 5:
				out = (t_%255&t_)-(t_>>13&t_);
				break;
				case 6:
				out = (t_-(t_>>4&t_>>8)&t_>>12)-1;
				break;
				case 7:
				out = ((t_*t_)/(t_^t_>>8))&t_;
				break;
				case 8:
				out = ((2*(t_&1)-1)*t_)-(t_>>8);
				break;
				case 9:
				out = t_*(t_>>(t_>>13&t_));
				break;
				case 10:
				out = (t_*t_/(1+(t_>>9&t_>>8)))&128;
				break;
				case 11:
				out = (t_*(-(t_>>8|t_|t_>>9|t_>>13)))^t_;
				break;
				case 12:
				out = (t_<<13)|(t_&t_>>3)*(t_>>11&t_)-(t_>>(t_^17))-2;
				break;
				case 13:
				out = (t_&t_>>4)-(t_>>13&t_);
				break;
				case 14:
				out = ((t_/1000)^(t_/1001))*t_;
				break;
				case 15:
				out = (((t_&t_>>8)-(t_>>13&t_))&((t_&t_>>8)-(t_>>13)))^(t_>>8&t_);
				break;
			}

			output_ = ((sample)out - 127.5) / 127.5;
			sample_count_ = 0;
		}

		return { output_ };
	}

	private:
	unsigned int t_ = 0;
	int sample_count_ = 0;
	sample output_ = 0.0;
    int formula_idx_ = 1;
};

MIN_EXTERNAL(byteplay_tilde);
