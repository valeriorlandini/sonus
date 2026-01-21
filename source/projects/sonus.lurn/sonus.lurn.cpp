/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace c74::min;

class lurn : public object<lurn>
{
public:
	MIN_DESCRIPTION {"Non repeating random sequence of random symbols"};
	MIN_TAGS {"random"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"urn, random"};

	inlet<>  in {this, "(bang) Next item"};
	inlet<>  in_l {this, "(list) Items to choose from"};
	outlet<> out {this, "(symbol) Output"};
	outlet<> out_end {this, "(bang) Bang when all items have been chosen"};

	lurn()
	{
		srand(time(0));
	}
	
	message<> anything
	{
		this,
		"anything",
		"On second inlet, list of items to choose from.",
        MIN_FUNCTION
		{
			if (inlet == 1)
			{
				elements_ = from_atoms<std::vector<std::string>>(args);
				elements_left_ = from_atoms<std::vector<std::string>>(args);
				if (elements_.size() > 0 && elements_.at(0) == "list")
				{
					elements_.erase(elements_.begin());
					elements_left_.erase(elements_left_.begin());
				}

				if (elements_.size() > 0)
				{
					all_out_ = false;
				}
			}

			return {};
		}
    };

	message<> clear
	{
		this,
		"clear",
		"Clear the list of already chosen items.",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				elements_left_ = elements_;

				if (elements_.size() > 0)
				{
					all_out_ = false;
				}
			}

			return {};
		}
    };

	message<> m_bang
	{
		this,
		"bang",
		"Output next item.",
		MIN_FUNCTION
		{
			if (inlet == 0)
			{
				if (all_out_)
				{
					out_end.send(k_sym_bang);
				}
				else
				{
					unsigned int element = rand() % elements_left_.size();
					out.send(elements_left_.at(element));
					elements_left_.erase(elements_left_.begin() + element);
					if (elements_left_.size() == 0)
					{
						all_out_ = true;
						out_end.send(k_sym_bang);
					}
				}
			}

			return {};
		}
	};

	private:
	std::vector<std::string> elements_;
	std::vector<std::string> elements_left_;
	bool all_out_ = true;
};

MIN_EXTERNAL(lurn);
