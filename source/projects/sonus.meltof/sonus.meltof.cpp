/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>
#include <vector>

using namespace c74::min;

class meltof : public object<meltof>
{
public:
	MIN_DESCRIPTION {"Mel to frequency converter"};
	MIN_TAGS {"math, utilities"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"ftom, mtof, sonus.ftomel"};

	inlet<>  in {this, "(number) Pitch (mel)"};
	outlet<> out {this, "(number) Frequency (Hz)"};

	
	enum class algos : int { shaugh, linor, slaney, enum_count };

    enum_map algos_range = {"O'Shaughnessy", "Lindsay & Norman", "Slaney"};

	attribute<algos> algo
	{
        this,
        "algorithm",
        algos::shaugh,
		algos_range,
        title {"Algorithm"},
		description {"Conversion algorithm."}
    };

    message<> m_number
	{
		this,
		"number",
		"Pitch",
        MIN_FUNCTION
		{
			number p = args[0];
			out.send(conversion(p));

			return {};
		}
    };

	message<> list
	{
		this,
		"list",
		"List of pitches to convert.",
        MIN_FUNCTION
		{
			atoms output;
			output.reserve(args.size());

			for (auto n = 0; n < args.size(); n++)
			{
				output.push_back(conversion(number(args[n])));
			}

			out.send(output);

			return {};
		}
    };

	number conversion(number p)
	{
		switch (algo)
		{
			case algos::shaugh:
			return 700.0 * (std::pow(10.0, p / 2595.0) - 1.0);
			break;
			case algos::linor:
			return (std::pow(10.0, p / 2410.0) - 1.0) / 0.0016;
			break;
			case algos::slaney:
			if (p < 15.0)
			{
				return (200.0 * p) / 3.0;
			}
			else
			{
				return std::pow(10.0, ((p - 15.0) / 27.0) * std::log10(6.4)) / 0.001;
			}
			break;
		}

		return 0.0;
	}
};

MIN_EXTERNAL(meltof);
