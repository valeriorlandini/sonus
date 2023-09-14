/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <string>
#include <vector>

using namespace c74::min;

template<class TSample>
struct fselect_info
{
    TSample gain_a;
    TSample gain_b;
    unsigned int a;
    unsigned int b;
};

class blist : public object<blist>
{
public:
	MIN_DESCRIPTION {"Output symbols by banging on inlets"};
	MIN_TAGS {"utlity, lists"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {""};

	outlet<> out {this, "(symbol) Output"};
/*
	argument<int> inputs_arg
    {
        this,
        "inputs",
        "The number of input channels."
    };
*/
    blist(const atoms& args = {})
    {
        inputs_ = 0;

        if (!args.empty())
        {
            inputs_ = std::min(int(args.size()), 256);
        }

	    for (auto i = 0; i < inputs_; ++i)
        {
            m_inlets.push_back(std::make_unique<inlet<>>(this, "(bang) Output " + std::string(args[i])));
        }
    }

private:
    int inputs_;
    std::vector<std::unique_ptr<inlet<>>> m_inlets;
};

MIN_EXTERNAL(blist);
