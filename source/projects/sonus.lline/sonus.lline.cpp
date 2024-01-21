/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <vector>
#include "interp.h"

using namespace c74::min;
using namespace soutel;

class lline : public object<lline>
{
public:
	MIN_DESCRIPTION {"Gradually ramp from the values of a number list to another"};
	MIN_TAGS {"envelope"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"line, line~"};

	inlet<>  in {this, "(list) Ramp time + next values of the list"};
	outlet<> out {this, "(list) Output"};
	outlet<> out_end {this, "(bang) Bang when ramp reaches destination"};

	timer<> metro
	{
		this,
        MIN_FUNCTION
		{
			if (ramp_ <= 1000000000)
			{
				atoms output;
				output.reserve(current_list_.size());

				for (auto n = 0; n < current_list_.size(); n++)
				{
					output.push_back(linip(current_list_.at(n), next_list_.at(n), ramp_ * 0.000000001));
				}

				out.send(output);

				metro.delay(grain);

				ramp_ += step_;
				if (ramp_ > 1000000000)
				{
					out_end.send(k_sym_bang);
					current_list_.clear();
					for (auto n = 0; n < next_list_.size(); n++)
					{
						current_list_.push_back(next_list_.at(n));
					}
					next_list_.clear();
					metro.stop();
				}
			}
            return {};
        }
    };

	attribute<number, threadsafe::no, limit::clamp> grain
	{
		this,
		"grain",
		20.0,
		range { 1.0, 10000.0 },
        title {"Time grain (ms)"},
        description {"Interval between two outputs, in milliseconds. Default is 20 ms."},
        setter
		{
			MIN_FUNCTION
			{
				return args;
        	}
		}
    };
	
	message<> list
	{
		this,
		"list",
		"Set the next values for the list and start the ramp.",
        MIN_FUNCTION
		{
			if (args.size() > 1)
			{
				if (ramp_ > 0 && ramp_ < 1000000000)
				{
					for (auto n = 0; n < current_list_.size(); n++)
					{
						current_list_.at(n) = linip(current_list_.at(n), next_list_.at(n), ramp_ * 0.000000001);
					}
				}
				ramp_ = 0;
				double actual_time = double(grain) * std::round(double(args[0]) / double(grain));
				step_ = (unsigned int)std::floor(1000000000.0 * double(grain) / std::max(double(grain), actual_time));
				next_list_.clear();
				for (auto n = 1; n < args.size(); n++)
				{
					next_list_.push_back(double(args[n]));
				}
				if (next_list_.size() < current_list_.size())
				{
					for (auto n = next_list_.size(); n < current_list_.size(); n++)
					{	
						next_list_.push_back(0.0);
					}
				}
				metro.delay(0.0);
			}
			return {};
		}
    };	

	message<> set
	{
		this,
		"set",
		"Set the current values for the list.",
        MIN_FUNCTION
		{
			if (args.size() >= 1)
			{
				current_list_.clear();
				for (auto n = 0; n < args.size(); n++)
				{
					current_list_.push_back(double(args[n]));
				}
			}

			return {};
		}
    };

	private:
	std::vector<double> current_list_ = {0.0};
	std::vector<double> next_list_ = {0.0};
	unsigned long ramp_ = 0;
	unsigned long step_ = 0;
};

MIN_EXTERNAL(lline);
