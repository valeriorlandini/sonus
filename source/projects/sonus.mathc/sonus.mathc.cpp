/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

class mathc : public object<mathc>
{
public:
	MIN_DESCRIPTION {"Mathematical constants"};
	MIN_TAGS {"math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {""};

	inlet<>  in {this, "(bang) Output value"};
	outlet<> out {this, "(float) Requested constant value"};

	enum class constants : int { pi, pi2, e, sqrt2, sqrt3, golden,
	                            ln2, apery, catalan, cahen,
								connective, euler, dottie, laplace,
								lemniscate, omega, wallis, enum_count };

    enum_map constants_range = {"pi", "pi2", "e", "sqrt2", "sqrt3", "golden",
	                            "ln2", "apery", "catalan", "cahen",
								"connective", "euler", "dottie", "laplace",
								"lemniscate", "omega", "wallis"};

    message<> m_bang
	{
		this,
		"bang",
		"Output chosen constant value",
        MIN_FUNCTION
		{
			output();
			return {};
		}
    };

	attribute<constants> constant
	{
        this,
        "constant",
        constants::pi,
		constants_range,
        title {"Constant"},
		description {"Constant to output."},
		setter
		{
			MIN_FUNCTION
			{
				return args;
			}
		}
    };

	void output()
	{
		switch (constant)
		{
			case constants::pi:
			out.send(3.1415926535);
			break;
			case constants::pi2:
			out.send(6.2831853071);
			break;
			case constants::e:
			out.send(2.7182818284);
			break;
			case constants::sqrt2:
			out.send(1.4142135623);
			break;
			case constants::sqrt3:
			out.send(1.7320508075);
			break;
			case constants::golden:
			out.send(1.6180339887);
			break;
			case constants::ln2:
			out.send(0.6931471805);
			break;
			case constants::connective:
			out.send(1.8477590650);
			break;
			case constants::wallis:
			out.send(2.0945514815);
			break;
			case constants::lemniscate:
			out.send(2.6220575542);
			break;
			case constants::euler:
			out.send(0.5772156649);
			break;
			case constants::omega:
			out.send(0.5671432904);
			break;
			case constants::apery:
			out.send(1.2020569031);
			break;
			case constants::catalan:
			out.send(0.9159655941);
			break;
			case constants::dottie:
			out.send(0.7390851332);
			break;
			case constants::laplace:
			out.send(0.6627434193);
			break;
			case constants::cahen:
			out.send(0.6434105462);
			break;
		}
	}
};

MIN_EXTERNAL(mathc);
