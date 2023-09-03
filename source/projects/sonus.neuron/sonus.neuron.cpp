/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <vector>

using namespace c74::min;

class neuron : public object<neuron>
{
public:
	MIN_DESCRIPTION {"Implementation of an artificial neuron"};
	MIN_TAGS {"neural networks, ai"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.perceptron~"};

	inlet<>  in {this, "(list) Inputs"};
	outlet<> out {this, "(float) Output"};

	attribute<std::vector<double>> weights
	{
		this,
		"weights",
		{1.0},
		title {"Weights"},
		description {"The weights."}
	};
	
	attribute<number> bias
	{
		this,
		"bias",
		0.0,
		title {"Bias"},
		description {"The bias."}
	};

	message<> list
	{
		this,
		"list",
		"Perform calculation on the list.",
        MIN_FUNCTION {
			auto x = from_atoms<std::vector<double>>(args);
			const std::vector<double>& wg = this->weights;
			int inputs = std::min(wg.size(), x.size());

			number result = 0.0;

			for (int i = 0; i < inputs; i++)
			{
				result += weights[i] * x[i];
			}

            out.send(result + bias);
            return {};
        }
    };
};

MIN_EXTERNAL(neuron);
