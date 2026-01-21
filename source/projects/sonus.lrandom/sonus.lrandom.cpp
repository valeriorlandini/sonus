/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace c74::min;

class lrandom : public object<lrandom>
{
public:
	MIN_DESCRIPTION {"Randomly choose items from a list"};
	MIN_TAGS {"random"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"zl.scramble, random"};

	inlet<>  in {this, "(bang) Next element"};
	inlet<>  in_l {this, "(list) Items to choose from"};
	outlet<> out {this, "(symbol) Output"};

	lrandom()
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
				if (elements_.size() > 0 && elements_.at(0) == "list")
				{
					elements_.erase(elements_.begin());
				}
			}

			return {};
		}
    };

	message<> append
	{
		this,
		"append",
		"Append elements to the list of items to choose from.",
		MIN_FUNCTION
		{
			if (inlet == 0)
			{
				std::vector<std::string> new_elements = from_atoms<std::vector<std::string>>(args);
				elements_.insert(elements_.end(), new_elements.begin(), new_elements.end());
			}

			return {};
		}
	};

	message<> erase
	{
		this,
		"erase",
		"Remove elements from the list of items to choose from.",
		MIN_FUNCTION
		{
			if (inlet == 0)
			{
				std::vector<std::string> del_elements = from_atoms<std::vector<std::string>>(args);
				for (auto element : del_elements)
				{
					auto it = std::find(elements_.begin(), elements_.end(), element);
					if (it != elements_.end())
					{
						elements_.erase(it);
					}
				}
			}

			return {};
		}
	};

	message<> clear
	{
		this,
		"clear",
		"Clear the list of items.",
        MIN_FUNCTION
		{
			if (inlet == 0)
			{
				elements_.clear();
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
			if (inlet == 0 && elements_.size() > 0)
			{
				unsigned int element = rand() % elements_.size();
				out.send(elements_.at(element));
			}

			return {};
		}
	};

	private:
	std::vector<std::string> elements_;
};

MIN_EXTERNAL(lrandom);
