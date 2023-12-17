/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "bytebeat_parser.h"
#include "c74_min.h"
#include <string>

using namespace c74::min;

class bytebeat_tilde : public object<bytebeat_tilde>, public sample_operator<0, 1>
{
public:
	MIN_DESCRIPTION {"Bytebeat music generator"};
	MIN_TAGS {"algorithmic, generator"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {""};

	inlet<>  in {this, "(signal) Input"};
	inlet<>  in_sr {this, "(int) Sample rate reduction factor (1-10)"};
	outlet<> out {this, "(signal) Output", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			return {};
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

	attribute<symbol, threadsafe::no> formula
	{
        this,
        "formula",
        "t",
        title {"Formula"},
        description {"Formula for the bytebeat generation. Time variable must be named t, check help file for allowed operators."},
		setter
		{
			MIN_FUNCTION
			{
				formula_ = "";
				for (auto a = 0; a < args.size(); a++)
				{
					formula_ += args[a];
				}
				for (auto t = 0; t < values_.size(); t++)
				{
					std::string formula_t = formula_;
					replace_t(formula_t, t);
					int value = 0;
					if (bb_parser_.parse(formula_t, value))
					{
						values_.at(t) = int(uint8_t(value));
					}
					else
					{
						values_.at(t) = 0;
					}
				}

				return {symbol(formula_)};
			}
		}
    };

	message<> m_number
	{
		this,
		"number",
		"In inlet 1, sample rate reduction factor",
        MIN_FUNCTION
		{
			if (inlet == 1)
			{
				sr_red = args[0];
			}

			return {};
		}
    };

	sample operator()()
    {
		++sample_count_;

		if (sample_count_ >= sr_red)
		{
			++t_;
			if (t_ >= values_.size())
			{
				t_ = 0;
			}

			output_ = ((sample)values_.at(t_) - 127.5) / 127.5;
			sample_count_ = 0;
		}

		return { output_ };
	}

	private:
	int t_ = 0;
	int sample_count_ = 0;
	std::string formula_;
	sample output_ = 0.0;
	std::array<int, 44100*4> values_;
	parser bb_parser_ = generate_parse<int>();

	inline void replace_t(std::string &input, const int &number)
	{
		size_t pos = 0;
		std::string target = "t";

		while ((pos = input.find(target, pos)) != std::string::npos)
		{
			input.replace(pos, target.length(), std::to_string(number));
        	pos += std::to_string(number).length();
	    }
	}
};

MIN_EXTERNAL(bytebeat_tilde);
