/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <cmath>
#include "window_functions.h"

using namespace c74::min;
using namespace soutel;

class window_tilde : public object<window_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Window functions"};
	MIN_TAGS {"math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {""};

	inlet<>  in {this, "(signal) Window Position (from 0 to 1)"};
	outlet<> out {this, "(signal) Result", "signal"};

	enum class windows : int { hann, hamming, blackman, nuttall, blackmannuttall, blackmanharris, flattop, bartletthann, rectangular, enum_count };

    enum_map windows_range = {"hann", "hamming", "blackman", "nuttall", "blackman-nuttall", "blackman-harris", "flat-top", "bartlett-hann", "rectangular"};

    attribute<windows> window
	{
		this,
		"window",
		windows::hann,
		windows_range,
		title {"Window"},
        description {"Window function to use, chosen among hann, hamming, blackman, nuttall, blackman-nuttall, blackman-harris, flat-top, bartlett-hann and rectangular."}
    };

	sample operator()(sample x)
    {	
		double output = x;

		switch (window)
		{
			case windows::hann:
			output = hann(x);
			break;
			case windows::hamming:
			output = hamming(x);
			break;
			case windows::blackman:
			output = blackman(x);
			break;
			case windows::nuttall:
			output = nuttall(x);
			break;
			case windows::blackmannuttall:
			output = blackmannuttall(x);
			break;
			case windows::blackmanharris:
			output = blackmanharris(x);
			break;
			case windows::flattop:
			output = flattop(x);
			break;
			case windows::bartletthann:
			output = bartletthann(x);
			break;
			case windows::rectangular:
			output = 1.0;
			break;
		}

		return output;
	}
};

MIN_EXTERNAL(window_tilde);
