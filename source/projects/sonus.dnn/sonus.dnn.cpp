/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "dnn.h"
#include "json.hpp"

using namespace c74::min;
using namespace deepnn;
using json = nlohmann::json;

class dnn : public object<dnn>
{
public:
	MIN_DESCRIPTION {"Deep neural network implementation"};
	MIN_TAGS {"ai"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.neuron, sonus.ca, sonus.linden"};

	inlet<>  in {this, "(list) Inputs"};
	outlet<> out {this, "(list) Outputs"};

	message<> load
	{
		this,
		"load",
       	"JSON with the network specification",
       	MIN_FUNCTION
		{
			if (args.empty())
			{
				return {};
			}

			std::string filename;
			for (auto a = 0; a < args.size(); a++)
			{
				filename += args[a];
				if (a < args.size() - 1)
				{
					filename += " ";
				}
			}

			std::ifstream json_file(filename);

    		if (!json_file.is_open())
			{
        		cerr << "Failed to open the JSON file." << endl;
        		return {};
    		}

    		std::string json_string((std::istreambuf_iterator<char>(json_file)), std::istreambuf_iterator<char>());

    		try
			{
        		json data = json::parse(json_string);
				network_.clear();

        		for (const auto& layer : data["layers"])
				{
					Layer<double> curr_layer(layer["output_size"]);

					int nc = 0;

            		for (const auto& ws : layer["weights"])
					{
						std::vector<double> curr_weights;
                		for (const auto& w : ws)
						{
							curr_weights.push_back(w);
                		}
						curr_layer.at(nc).weights = curr_weights;
						curr_layer.at(nc).bias = layer["biases"][nc];
						curr_layer.at(nc).set_activation(layer["activation"]);
						nc++;
            		}

					network_.push_layer(curr_layer);
        		}
    		}
			catch (json::parse_error& e)
			{
        		cerr << "Parsing error: " << e.what() << endl;
        		return {};
    		}

        	return {};
       }
    };

	message<> list
	{
		this,
		"list",
		"Input values for the neural network.",
        MIN_FUNCTION
		{
			if (!args.empty())
			{
				std::vector<double> input;
				for (auto n = 0; n < args.size(); n++)
				{
					input.push_back(double(args[n]));
				}

				std::vector<double> output = network_.run(input);

				out.send(to_atoms(output));
			}
			return {};
		}
    };

	message<> m_number
	{
		this,
		"number",
		"Input value for the neural network, if only one value is expected.",
        MIN_FUNCTION
		{
			std::vector<double> input = {double(args[0])};

			std::vector<double> output = network_.run(input);

			out.send(to_atoms(output));

			return {};
		}
    };

	private:
	DNN<double> network_;
};

MIN_EXTERNAL(dnn);
