/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Sonus Dept. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <cmath>

using namespace c74::min;

enum Activations
{
	RELU,
	SIGMOID,
	TANH,
	SOFTPLUS,
	SELU,
	LEAKYRELU,
	SILU,
	GAUSS,
	BINARY
};

class acti : public object<acti>
{
public:
	MIN_DESCRIPTION {"Activation functions"};
	MIN_TAGS {"neural networks, math"};
	MIN_AUTHOR {"Sonus Dept."};
	MIN_RELATED {"sonus.acti~"};

	inlet<>  in {this, "(number) Input"};
	outlet<> out {this, "(float) Result"};

    message<> m_number
	{
		this,
		"number",
		"Input",
        MIN_FUNCTION
		{
			output((double)args[0]);
			return {};
		}
    };

	enum class activations : int { relu, sigmoid, tanh, softplus, selu, leakyrelu, silu, binary, gauss, enum_count };

    enum_map activations_range = {"relu", "sigmoid", "tanh", "softplus", "selu", "leakyrelu", "silu", "binary", "gauss"};

    attribute<activations> activation
	{
		this,
		"activation",
		activations::relu,
		activations_range,
		title {"Activation"},
        description {"Activation function to use, chosen among relu, sigmoid, tanh, softplus, selu, leakyrelu, silu, binary and gauss."}
    };

	void output(double x)
	{
		switch (activation)
		{
			case activations::relu:
			out.send(std::max(0.0, x));
			break;
			case activations::leakyrelu:
			out.send(std::max(0.1 * x, x));
			break;
			case activations::sigmoid:
			out.send(1.0 / (1.0 + exp(-1.0 * x)));
			break;
			case activations::tanh:
			out.send((exp(x) - exp(-1.0 * x)) / (exp(x) + exp(-1.0 * x)));
			break;
			case activations::softplus:
			out.send(log(1.0 + exp(x)));
			break;
			case activations::selu:
			if (x > 0.0)
			{
				out.send(1.050701 * x);
			}
			else
			{
				out.send(1.7581 * (exp(x) - 1.0));
			}
			break;
			case activations::silu:
			out.send(x / (1.0 + exp(-1.0 * x)));
			break;
			case activations::binary:
			out.send((double)(x > 0.0));
			break;
			case activations::gauss:
			out.send(exp(-1.0 * (x * x)));
			break;
		}
	}
};

MIN_EXTERNAL(acti);
