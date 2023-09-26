/******************************************************************************
Copyright (c) 2020 Valerio Orlandini

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef GENEALGO_H_
#define GENEALGO_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <ctime>
#include <iostream>
#include <random>
#include <utility>
#include <vector>

template<class T>
struct chromosome
{
	std::vector<T> sequence;
	unsigned int fitness;
};


template <class T>
class GeneAlgo
{
public:
	GeneAlgo()
	{
		generator.seed(time(NULL));
		target_reached_ = false;
		set_mutation_rate(0.01);
		set_mating_rate(0.1);
	}

	~GeneAlgo()
	{

	}

	bool populate(const std::vector<T>& dictionary, unsigned int population_size = 16, unsigned int chromosome_length = 16)
	{
		if (dictionary.size() == 0)
		{
			std::cerr << "Empty dictionary!\n";

			return false;
		}
		else
		{
			dictionary_ = dictionary;
		}

		// Population must have at least 2 chromosomes, chromosomes must have at least 2 bases
		if (population_size * chromosome_length < 4)
		{
			population_size < 2 ? std::cerr << "Empty or size 1 population\n" : std::cerr << "Empty or length 1 chromosome\n";

			return false;
		}

		std::uniform_int_distribution<int> distribution(0, dictionary_.size() - 1);

		population_.clear();

		for (unsigned int p = 0; p < population_size; p++)
		{
			chromosome<T> curr_chr;

			for (unsigned int c = 0; c < chromosome_length; c++)
			{
				curr_chr.sequence.push_back(dictionary_.at(distribution(generator)));
			}

			curr_chr.fitness = 0;

			population_.push_back(curr_chr);
		}

		population_size_ = population_size;

		target_reached_ = false;

		return true;
	}

	bool set_target(std::vector<T> target, bool adj_chr_sizes = false)
	{
		if (target.size() == population_.at(0).sequence.size())
		{
			target_ = target;

			return true;
		}
		else if (adj_chr_sizes && target.size() > 1)
		{
			target_ = target;

			if (target_.size() < population_.at(0).sequence.size())
			{
				for (unsigned int c = 0; c < population_.size(); c++)
				{
					while (target_.size() < population_.at(c).sequence.size())
					{
						population_.at(c).sequence.pop_back();
					}
				}
			}
			else
			{
				std::uniform_int_distribution<int> distribution(0, dictionary_.size() - 1);

				for (unsigned int c = 0; c < population_.size(); c++)
				{
					while (target_.size() > population_.at(c).sequence.size())
					{
						population_.at(c).sequence.push_back(dictionary_.at(distribution(generator)));
					}
				}

			}

			return true;
		}

		target.size() < 2 ? std::cerr << "Empty or length 1 target\n" : std::cerr << "Target sequence length is not coherent with cromosome length\n";

		return false;
	}

	bool crossover(unsigned int parent_1, unsigned int parent_2, chromosome<T>& child_1, chromosome<T>& child_2, const bool &multiple_crossover = false)
	{
		if (parent_1 < population_.size() && parent_2 < population_.size())
		{
			child_1.sequence = population_.at(parent_1).sequence;
			child_2.sequence = population_.at(parent_2).sequence;

			unsigned int cos;
			multiple_crossover ? cos = (unsigned int)std::floor((float)child_1.sequence.size() * 0.125) : cos = 1;

			for (unsigned int c = 0; c < cos; c++)
			{
				std::uniform_int_distribution<int> distribution_1(0, child_1.sequence.size() - 2);

				unsigned int crossover_1 = distribution_1(generator);

				std::uniform_int_distribution<int> distribution_2(crossover_1, child_1.sequence.size() - 1);

				unsigned int crossover_2 = distribution_2(generator);

				std::swap_ranges(child_1.sequence.begin() + crossover_1, child_1.sequence.begin() + crossover_2, child_2.sequence.begin() + crossover_1);
			}

			return true;
		}

		std::cerr << "Parent does not exist\n";

		return false;
	}

	bool mutation(chromosome<T>& chr_mut)
	{
		if (chr_mut.sequence.size() != 0)
		{
			std::uniform_int_distribution<int> distribution_c(0, chr_mut.sequence.size() - 1);
			std::uniform_int_distribution<int> distribution_d(0, dictionary_.size() - 1);

			chr_mut.sequence.at(distribution_c(generator)) = dictionary_.at(distribution_d(generator));

			return true;
		}

		std::cerr << "Empty chromosome\n";

		return false;
	}

	bool fitness(bool& target_reached)
	{
		target_reached = false;

		for (unsigned int i = 0; i < population_.size(); i++)
		{
			unsigned int score = 1;

			for (unsigned int t = 0; t < target_.size(); t++)
			{
				if (population_.at(i).sequence.at(t) == target_.at(t))
				{
					++score;
				}
			}

			if (score == target_.size() + 1)
			{
				target_reached = true;
			}

			population_.at(i).fitness = score;
		}

		return true;
	}

	void selection(unsigned int& parent_1_id, unsigned int& parent_2_id)
	{
		std::vector<double> scores;

		for (unsigned int c = 0; c < population_.size(); c++)
		{
			scores.push_back((double)population_.at(c).fitness);
		}

		std::discrete_distribution<int> distribution_d(scores.begin(), scores.end());
		parent_1_id = distribution_d(generator);
		parent_2_id = parent_1_id;

		while (parent_2_id == parent_1_id)
		{
			parent_2_id = distribution_d(generator);
		}
	}

	void sort_population()
	{
		std::sort(population_.begin(), population_.end(), [](const auto& i, const auto& j) { return i.fitness > j.fitness; });
	}

	void kill_unfit()
	{
		while (population_.size() > population_size_)
		{
			unsigned int begin = (unsigned int)(std::floor((float)population_.size() * 0.3));
			std::uniform_int_distribution<int> distribution_k(begin, population_.size() - 1);
			auto position_k = population_.begin() + distribution_k(generator);
			population_.erase(position_k);
		}
	}

	inline bool evolution(const bool &multiple_crossover = false)
	{
		fitness(target_reached_);

		sort_population();


		for (int m = 0; m < (int)ceilf(mutation_rate_ * (float)population_.size()); m++)
		{
			std::uniform_int_distribution<int> distribution_p(0, population_.size() - 1);
			mutation(population_.at(distribution_p(generator)));
		}

		std::vector<chromosome<T>> children;

		for (int t = 0; t < (int)ceilf(mating_rate_ * (float)population_size_); t++)
		{
			unsigned int parent_1_id, parent_2_id;
			selection(parent_1_id, parent_2_id);
			chromosome<T> child_1, child_2;
			crossover(parent_1_id, parent_2_id, child_1, child_2, multiple_crossover);
			population_.push_back(child_1);
			population_.push_back(child_2);
		}

		fitness(target_reached_);

		sort_population();

		kill_unfit();

		return target_reached_;
	}

	std::vector<chromosome<T>> get_population()
	{
		return population_;
	}

	unsigned int check_dict_target()
	{
		unsigned int not_present = 0;
		for (unsigned int t = 0; t < target_.size(); t++)
		{
			if (std::find(dictionary_.begin(), dictionary_.end(), target_.at(t)) == dictionary_.end())
			{
				++not_present;
			}
		}

		return not_present;
	}

	bool set_population(std::vector<std::vector<T>> sequences)
	{
		if (sequences.size() > 1)
		{
			population_.clear();

			for (int s = 0; s < sequences.size(); s++)
			{
				chromosome<T> new_chr;
				new_chr.sequence = sequences.at(s);
				new_chr.fitness = 1;
				population_.push_back(new_chr);
			}

			population_size_ = population_.size();

			return true;
		}

		std::cerr << "Empty or size 1 population\n";

		return false;
	}

	chromosome<T> get_chromosome(unsigned int chromosome_id)
	{
		if (chromosome_id < population_.size())
		{
			return population_.at(chromosome_id);
		}

		std::cerr << "Chromosome out of range\n";

		return {};
	}

	std::vector<T> get_target()
	{
		return target_;
	}

	bool set_mutation_rate(float mutation_rate)
	{
		if (mutation_rate >= 0.0)
		{
			mutation_rate_ = mutation_rate;

			return true;
		}

		std::cerr << "Mutation rate less than zero\n";

		return false;
	}

	bool set_mating_rate(float mating_rate)
	{
		if (mating_rate >= 0.0)
		{
			mating_rate_ = mating_rate;

			return true;
		}

		std::cerr << "Mating rate less than zero\n";

		return false;
	}

	bool set_dictionary(std::vector<T> dictionary)
	{
		if (dictionary.size() > 0)
		{
			dictionary_ = dictionary;

			return true;
		}

		std::cerr << "Empty dictionary\n";

		return false;
	}

private:
	std::vector<chromosome<T>> population_;
	std::vector<unsigned int> fitness_order_;
	std::vector<T> dictionary_;
	std::vector<T> target_;

	float mutation_rate_;
	float mating_rate_;
	unsigned int population_size_;
	bool target_reached_;

	std::default_random_engine generator;
};

#endif // GENEALGO_H_
