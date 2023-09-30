/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "../include/genealgo.h"

using namespace c74::min;

class ga_num : public object<ga_num>
{
public:
	MIN_DESCRIPTION {"Genetic algorithm implementation (numbers output version)"};
	MIN_TAGS {"ai, math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.ca, sonus.ga, sonus.linden"};

	inlet<>  in {this, "(bang) Evolve chromosomes"};
	outlet<> out_p {this, "(list) Current population", "list"};
	outlet<> out_f {this, "(list) Current fittest", "list"};
	outlet<> out_r {this, "(bang) Bang when target reached"};

	message<> dictionary
	{
		this,
		"dictionary",
       	"Dictionary with the system definition",
       	MIN_FUNCTION
		{
			try
			{
				target_reached_ = false;
				dict system = {args[0]};
				atoms d_target = system["target"];
				atoms d_range = system["range"];
				int lowest = 0;
				int highest = 0;
				if (d_range.size() > 1)
				{
					lowest = int(d_range[0]);
					highest = int(d_range[1]);
					if (highest - lowest < 1)
					{
						highest = lowest + 1;
					}
				}
				else
				{
					highest = std::min(std::max(2, int(d_range[0])), 1024);
				}
				std::vector<unsigned int> dictionary;
				for (auto c = lowest; c <= highest; c++)
				{
					dictionary.push_back(c);
				}
				atoms d_pop_size = system["population"];
				unsigned int pop_size = int(d_pop_size[0]);
				if (pop_size < 2)
				{
					pop_size = 16;
				}
				if (d_target.size() > 1)
				{
					std::vector<unsigned int> target;
					for (auto c = 0; c < d_target.size(); c++)
					{
						target.push_back(int(d_target[c]));
					}	

					unsigned int chromosome_length = target.size();
					genome_.populate(dictionary, pop_size, chromosome_length);
					genome_.set_target(target);

					set_ = true;
				}
				else
				{
					cerr << "Target not set or too short (must be at least two numbers)" << endl;
				}
           	}
           	catch (std::exception& e)
			{
            	cerr << e.what() << endl;
           	}
        	return {};
       }
    };

	attribute<number, threadsafe::no, limit::clamp> mutrate
	{
		this,
		"mutrate",
		0.05,
		range { 0.0, 1.0 },
        title {"Mutation rate (0-1)"},
        description {"Probability of mutation in the population."},
        setter
		{
			MIN_FUNCTION
			{
				genome_.set_mutation_rate(args[0]);
				return args;
        	}
		}
    };

	attribute<number, threadsafe::no, limit::clamp> matrate
	{
		this,
		"matrate",
		0.15,
		range { 0.0, 1.0 },
        title {"Mating rate (0-1)"},
        description {"Proportion of population members that mate at every step."},
        setter
		{
			MIN_FUNCTION
			{
				genome_.set_mating_rate(args[0]);
				return args;
        	}
		}
    };

	attribute<bool, threadsafe::no> multicross
	{
		this,
		"multicross",
		false,
        title {"Multiple crossovers"},
        description {"Allow multiple crossovers during mating."},
        setter
		{
			MIN_FUNCTION
			{
				return args;
        	}
		}
    };

	message<> bang
	{
		this,
		"bang",
		"Evolve the genome.",
        MIN_FUNCTION
		{
			if (!target_reached_ && set_)
			{
				target_reached_ = genome_.evolution(multicross);
				std::vector<chromosome<unsigned int>> population = genome_.get_population();
				atoms a_population;
				a_population.reserve(population.size());
				atoms a_bestfit;
				a_bestfit.reserve(population.at(0).sequence.size());

				for (auto c = 0; c < population.size(); c++)
				{
					std::vector<unsigned int> curr_vector = population.at(c).sequence;
					std::string curr_symbol = "";
					for (auto a = 0; a < curr_vector.size(); a++)
					{
						curr_symbol += std::to_string(curr_vector.at(a));
						if (a < curr_vector.size() - 1)
						{
							curr_symbol += " ";
						}
						
						if (c == 0)
						{
							a_bestfit.push_back(curr_vector.at(a));
						}
					}
					a_population.push_back(curr_symbol);
				}
				out_f.send(a_bestfit);
				out_p.send(a_population);
			}

			if (target_reached_)
			{
				out_r.send(k_sym_bang);
			}
            return {};
        }
    };

	private:
	GeneAlgo<unsigned int> genome_;
	bool target_reached_ = false;
	bool set_ = false;
};

MIN_EXTERNAL(ga_num);
