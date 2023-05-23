/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Sonus Dept. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;

class perceptron : public object<perceptron>, public sample_operator<3, 2>
{
public:
	MIN_DESCRIPTION {"Implementation of the classic perceptron algorithm"};
	MIN_TAGS {"audio, neural networks"};
	MIN_AUTHOR {"Sonus Dept."};
	MIN_RELATED {"sonus.neuron"};

	inlet<>  in1 {this, "(signal) Input"};
	inlet<>  in_w {this, "(signal) Weight"};
	inlet<>  in_b {this, "(signal) Bias"};
	outlet<> out {this, "(signal) Binary result (0 or 1)", "signal"};
	outlet<> out_v {this, "(signal) Value", "signal"};

	samples<2> operator()(sample input, sample w = 1.0, sample b = 0.0)
    {
		auto value = input * w + b;
        auto output = (value > 0.0) ? 1.0 : 0.0;

		return { {output, value} };
	}
};

MIN_EXTERNAL(perceptron);
