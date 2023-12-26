/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "dnn.h"

using namespace c74::min;
using namespace dnn;

class dnn : public object<dnn>
{
public:
	MIN_DESCRIPTION {"Deep neural network implementation"};
	MIN_TAGS {"ai"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.neuron, sonus.ca, sonus.linden"};

	inlet<>  in {this, "(list) Inputs"};
	outlet<> out {this, "(list) Outputs"};

	message<> dictionary
	{
		this,
		"dictionary",
       	"Dictionary with the system definition",
       	MIN_FUNCTION
		{
			try
			{
				dict system_ = {args[0]};
				atoms rules = system_["rules"];
				atoms axiom = system_["axiom"];
				generation_ = std::string(axiom[0]);
				rules_.clear();

				if (!(rules.size() % 2))
				{
					for (auto r = 0; r < rules.size(); r += 2)
					{
						rule new_rule;
						std::string in_str = rules[r];
						new_rule.input = in_str.at(0);
						new_rule.output = std::string(rules[r + 1]);
						rules_.push_back(new_rule);
					}

					out.send(generation_);
				}
				else
				{
					cerr << "Rules elements must be even" << endl;
				}
           	}
           	catch (std::exception& e)
			{
            	cerr << e.what() << endl;
           	}
        	return {};
       }
    };

	message<> curr
	{
        this,
        "curr",
        "Output the current generation",
        MIN_FUNCTION
        {
            out.send(generation_);

			return {};
		}
	};

	message<> bang
	{
		this,
		"bang",
		"Output next generation.",
        MIN_FUNCTION
		{
			std::string next_generation = "";
			for (auto c = 0; c < generation_.size(); c++)
			{
				char curr = generation_.at(c);

				bool found = false;
				for (auto r = 0; r < rules_.size(); r++)
				{
					if (rules_.at(r).input == curr)
					{
						next_generation += rules_.at(r).output;
						found = true;
						break;
					}
				}

				if (!found)
				{
					next_generation += curr;
				}
			}
			generation_ = next_generation;

			out.send(next_generation);

            return {};
        }
    };

	private:
	DNN<double> network_;
	std::string generation_ = "";
	std::vector<rule> rules_;
};

MIN_EXTERNAL(dnn);
