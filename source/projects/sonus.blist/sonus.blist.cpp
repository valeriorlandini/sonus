/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
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

	outlet<> out {this, "(symbol / float) Output"};

    blist(const atoms& args = {})
    {
        inputs_ = 0;

        if (!args.empty())
        {
            inputs_ = std::min(int(args.size()), 256);
            symbols_ = from_atoms<std::vector<std::string>>(args);
        }

	    for (auto i = 0; i < inputs_; ++i)
        {
            m_inlets.push_back(std::make_unique<inlet<>>(this, "(bang) Output " + std::string(args.at(i))));
        }
    }
    
    attribute<bool, threadsafe::no> numout
	{
        this,
        "numout",
        false,
        title {"Number mode"},
        description {"If enabled, output numbers instead of symbols."},
		setter
		{
			MIN_FUNCTION
			{
				return args;
			}
		}
    };
    
    message<> m_bang
	{
		this,
		"bang",
		"Output corresponding symbol or number",
		MIN_FUNCTION
		{
            if (numout)
            {
                try
                {
                    out.send(std::stod(symbols_.at(inlet)));
                }
                catch (...)
                {
                    out.send(0.0);
                }
            }
            else
            {
                out.send(symbols_.at(inlet));
            }
			return {};
		}
	};

private:
    int inputs_;
    std::vector<std::string> symbols_;
    std::vector<std::unique_ptr<inlet<>>> m_inlets;
};

MIN_EXTERNAL(blist);
