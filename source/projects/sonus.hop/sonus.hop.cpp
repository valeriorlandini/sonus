/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

enum Operations
{
	ADD,
	SUB,
	MUL,
	DIV,
	ISUB,
	IDIV,
	POW,
	MOD
};

class hop : public object<hop>
{
public:
	MIN_DESCRIPTION {"Hot inlets math operators"};
	MIN_TAGS {"math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"expr, pow"};

	inlet<>  in_x {this, "(number) Left operand"};
	inlet<>  in_y {this, "(number) Right operand"};
	outlet<> out {this, "(float) Result"};

	enum class operators : int { add, sub, mul, div, inv_sub, inv_div, pow, mod, eq, neq, gt, get, lt, let, enum_count };

    enum_map operators_range = {"+", "minus (-)", "*", "/", "!-", "!/", "^", "%", "==", "!=", ">", ">=", "<", "<="};


	argument<symbol> operator_arg
	{
		this,
		"operator",
		"Operator to use.",
        MIN_ARGUMENT_FUNCTION
		{
			if (arg == "+")
			{
				op = operators::add;
			}
			if (arg == "-")
			{
				op = operators::sub;
			}
			if (arg == "*")
			{
				op = operators::mul;
			}
			if (arg == "/")
			{
				op = operators::div;
			}
			if (arg == "!-")
			{
				op = operators::inv_sub;
			}
			if (arg == "!/")
			{
				op = operators::inv_div;
			}
			if (arg == "^")
			{
				op = operators::pow;
			}
			if (arg == "%")
			{
				op = operators::mod;
			}
			if (arg == "==")
			{
				op = operators::eq;
			}
			if (arg == "!=")
			{
				op = operators::neq;
			}
			if (arg == ">")
			{
				op = operators::gt;
			}
			if (arg == ">=")
			{
				op = operators::get;
			}
			if (arg == "<")
			{
				op = operators::lt;
			}
			if (arg == "<=")
			{
				op = operators::let;
			}
        }
    };

    message<> m_number
	{
		this,
		"number",
		"Operand",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				x_ = args[0];
			}
			else
			{
				y_ = args[0];
			}

			output();
			return {};
		}
    };

	attribute<operators> op
	{
        this,
        "operator",
        operators::add,
		operators_range,
        title {"Operator"},
		description {"Operator to use."},
		setter
		{
			MIN_FUNCTION
			{
				output();
				return args;
			}
		}
    };

	void output()
	{
		switch (op)
		{
			case operators::add:
			out.send(x_ + y_);
			break;
			case operators::sub:
			out.send(x_ - y_);
			break;
			case operators::mul:
			out.send(x_ * y_);
			break;
			case operators::div:
			out.send(x_ / y_);
			break;
			case operators::inv_sub:
			out.send(y_ - x_);
			break;
			case operators::inv_div:
			out.send(y_ / x_);
			break;
			case operators::pow:
			out.send(pow(x_, y_));
			break;
			case operators::mod:
			out.send(fmod(x_, y_));
			break;
			case operators::eq:
			x_ == y_ ? out.send(1.0) : out.send(0.0);
			break;
			case operators::neq:
			x_ != y_ ? out.send(1.0) : out.send(0.0);
			break;
			case operators::gt:
			x_ > y_ ? out.send(1.0) : out.send(0.0);
			break;
			case operators::get:
			x_ >= y_ ? out.send(1.0) : out.send(0.0);
			break;
			case operators::lt:
			x_ < y_ ? out.send(1.0) : out.send(0.0);
			break;
			case operators::let:
			x_ <= y_ ? out.send(1.0) : out.send(0.0);
			break;
		}
	}

private:
	float x_ = 0.0;
	float y_ = 0.0;
	Operations operation_ = ADD;
};

MIN_EXTERNAL(hop);
