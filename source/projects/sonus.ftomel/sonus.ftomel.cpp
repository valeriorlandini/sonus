/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>

using namespace c74::min;

class ftomel : public object<ftomel>
{
public:
	MIN_DESCRIPTION {"Frequency to mel converter"};
	MIN_TAGS {"math, utilities"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"ftom, mtof, sonus.meltof"};

	inlet<>  in {this, "(number) Frequency (Hz)"};
	outlet<> out {this, "(number) Pitch (mel)"};

	
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
		"Frequency",
        MIN_FUNCTION
		{
			number f = args[0];
			switch (algo)
			{
				case algos::shaugh:
				out.send(2595.0 * std::log10(1.0 + (f / 700.0)));
				break;
				case algos::linor:
				out.send(2410.0 * std::log10((0.0016 * f) + 1.0));
				break;
				case algos::slaney:
				if (f < 1000.0)
				{
					out.send((3.0 * f) / 200.0);
				}
				else
				{
					out.send(15.0 + 27 * (std::log10(f * 0.001) / std::log10(6.4)));
				}
				break;
			}
			return {};
		}
    };
};

MIN_EXTERNAL(ftomel);
