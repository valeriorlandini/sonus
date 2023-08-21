/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <cmath>

using namespace c74::min;

class acti_tilde : public object<acti_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Activation functions"};
	MIN_TAGS {"neural networks, math, ai"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.acti"};

	inlet<>  in {this, "(signal) Input"};
	outlet<> out {this, "(signal) Result", "signal"};

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

	sample operator()(sample x)
    {	
		double output = x;

		switch (activation)
		{
			case activations::relu:
			output = std::max(0.0, x);
			break;
			case activations::leakyrelu:
			output = std::max(0.1 * x, x);
			break;
			case activations::sigmoid:
			output = 1.0 / (1.0 + exp(-1.0 * x));
			break;
			case activations::tanh:
			output = (exp(x) - exp(-1.0 * x)) / (exp(x) + exp(-1.0 * x));
			break;
			case activations::softplus:
			output = log(1.0 + exp(x));
			break;
			case activations::selu:
			if (x > 0.0)
			{
				output = 1.050701 * x;
			}
			else
			{
				output = 1.7581 * (exp(x) - 1.0);
			}
			break;
			case activations::silu:
			output = x / (1.0 + exp(-1.0 * x));
			break;
			case activations::binary:
			output = x > 0.0 ? 1.0 : 0.0;
			break;
			case activations::gauss:
			output = exp(-1.0 * (x * x));
			break;
		}

		return (sample)output;
	}
};

MIN_EXTERNAL(acti_tilde);
