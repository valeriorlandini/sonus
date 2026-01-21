/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <chrono>
#include <random>
#include <string>
#include <vector>

using namespace c74::min;

class multirandom : public object<multirandom>
{
public:
	MIN_DESCRIPTION {"A multi-outlet version of the classic random object"};
	MIN_TAGS {"math, utilities"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"random"};
    
	inlet<>  in {this, "(bang) Output random numbers"};
	inlet<>  in_r {this, "(number) Random number range"};

	argument<int> outputs_arg
    {
        this,
        "outputs",
        "The number of outputs."
    };

	argument<number> range_arg
    {
        this,
        "range",
        "Initial limit."
    };

	argument<int> seed_arg
    {
        this,
        "seed",
        "Random generator seed value."
    };

    attribute<bool, threadsafe::no> floatoutput
    {
        this,
        "floatoutput",
        false,
        title {"Generate floats"},
        description {"Generate floating point values instead of integers."}
    };

    attribute<long, threadsafe::no> seed
    {
        this,
        "seed",
        false,
        title {"Random seed"},
        description {"The random seed for the random generation."},
        setter
		{
			MIN_FUNCTION
			{
				gen_.seed(long(args[0]));
				return args;
        	}
		}
    };

    attribute<std::vector<number>> range
    {
        this,
        "range",
        {0, 1},
        title {"Range"},
        description {"The range of generated values."},
        setter
		{
			MIN_FUNCTION
			{
                if (args.size() > 0)
				{
                    number a = 0;
                    number b = 0;
					if (args.size() < 2)
                    {
                        b = number(args[0]);
                    }
                    else
                    {
                        a = number(args[0]);
                        b = number(args[1]);
                    }

                    if (b < a)
                    {
                        number temp = b;
                        b = a;
                        a = temp;
                    }
                    idist_ = std::uniform_int_distribution<>((int)a, (int)b);
                    fdist_ = std::uniform_real_distribution(a, b);
					
					return {a, b};
				}

				return args;
        	}
		}
    };

    multirandom(const atoms& args = {})
    {
        int outputs = 2;

        if (!args.empty() && int(args[0]) > 1)
        {
            outputs = std::min(int(args[0]) + 1, 257);
        }

        if (args.size() > 1)
        {
            number limit = number(args[1]);
            if (limit < 0)
            {
                range[0] = limit;
                range[1] = 0;
            }
            else
            {
                range[1] = limit;            
            }

            range.set(range);
        }

        if (args.size() > 2)
        {
            seed = int(args[2]);
        }
        else
        {
            const auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            seed = millis;
        }

	    for (auto i = 0; i < outputs; ++i)
        {
            if (i < outputs - 1)
            {
                m_outlets.push_back(std::make_unique<outlet<>>(this, "(number) Random number " + std::to_string(i+1)));
            }
            else
            {
                m_outlets.push_back(std::make_unique<outlet<>>(this, "(list) List with all numbers"));
            }
        }
    }

    message<> m_number
	{
		this,
		"number",
		"In second inlet, range limit",
        MIN_FUNCTION
		{
			if (inlet == 1)
            {
                number limit = number(args[0]);
                atoms limits;
                limits.push_back(0);
                limits.push_back(limit);
                this->range.set(limits);
            }
			return {};
		}
    };

    message<> bang
    {
        this,
        "bang",
        "Output a new series of random numbers",
        MIN_FUNCTION
        {
            if (floatoutput)
            {
                std::vector<number> generation;

                for (auto i = 0; i < m_outlets.size() - 1; i++)
                {   
                    generation.push_back(fdist_(gen_));
                    m_outlets.at(i)->send(generation.at(i));
                }

                atoms nums;
		        nums.reserve(generation.size());
    	        for (auto &n : generation)
    	        {
                    nums.push_back(n);
                }
        		m_outlets.at(m_outlets.size() - 1)->send(nums);
            }
            else
            {
                std::vector<int> generation;

                for (auto i = 0; i < m_outlets.size() - 1; i++)
                {   
                    generation.push_back(idist_(gen_));
                    m_outlets.at(i)->send(generation.at(i));
                }

                atoms nums;
		        nums.reserve(generation.size());
    	        for (auto &n : generation)
    	        {
                    nums.push_back(n);
                }
        		m_outlets.at(m_outlets.size() - 1)->send(nums);
            }
			return {};
        }
    };

private:
    std::vector<std::unique_ptr<outlet<>>> m_outlets;
    std::default_random_engine gen_;
    std::uniform_real_distribution<number> fdist_;
    std::uniform_int_distribution<int> idist_;
};

MIN_EXTERNAL(multirandom);
