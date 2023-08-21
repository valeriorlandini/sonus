/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <algorithm>
#include <cmath>

using namespace c74::min;

class window_tilde : public object<window_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Window functions"};
	MIN_TAGS {"math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.window"};

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
			output = 0.5 * (1.0 - cos(2.0 * M_PI * x));
			break;
			case windows::hamming:
			output = 0.54347826 * (1.0 - cos(2.0 * M_PI * x));
			break;
			case windows::blackman:
			output = 0.42659 - 0.49656 * cos(2.0 * M_PI * x) + 0.076849 * cos(4.0 * x);
			break;
			case windows::nuttall:
			output = 0.355768 - 0.487396 * cos(2.0 * M_PI * x) + 0.144232 * cos(4.0 * M_PI * x) - 0.012604 * cos(6.0 * M_PI * x);
			break;
			case windows::blackmannuttall:
			output = 0.3635819 - 0.4891775 * cos(2.0 * M_PI * x) + 0.1365995 * cos(4.0 * M_PI * x) - 0.0106411 * cos(6.0 * M_PI * x);
			break;
			case windows::blackmanharris:
			output = 0.35875 - 0.48829 * cos(2.0 * M_PI * x) + 0.14128 * cos(4.0 * M_PI * x) - 0.01168 * cos(6.0 * M_PI * x);
			break;
			case windows::flattop:
			output = 0.21557895 - 0.41663158 * cos(2.0 * M_PI * x) + 0.277263158 * cos(4.0 * M_PI * x) - 0.083578947 * cos(6.0 * M_PI * x) + 0.006947368 * cos(8.0 * M_PI * x);
			break;
			case windows::bartletthann:
			output = 0.62 - 0.48 * abs(x - 0.5) - 0.38 * cos(2.0 * M_PI * x);
			break;
			case windows::rectangular:
			output = 1.0;
			break;
		}

		return output;
	}
};

MIN_EXTERNAL(window_tilde);
