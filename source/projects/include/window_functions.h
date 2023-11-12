/******************************************************************************
Copyright (c) 2023 Valerio Orlandini

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/

#ifndef WINDOW_FUNCTIONS_H_
#define WINDOW_FUNCTIONS_H_

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template <class TSample>
inline TSample hann(const TSample &x)
{
    return 0.5 * (1.0 - cos(2.0 * M_PI * x));
}

template <class TSample>
inline TSample hamming(const TSample &x)
{
	return 0.54347826 * (1.0 - cos(2.0 * M_PI * x));
}

template <class TSample>
inline TSample blackman(const TSample &x)
{
	return 0.42659 - 0.49656 * cos(2.0 * M_PI * x) + 0.076849 * cos(4.0 * x);
}

template <class TSample>
inline TSample nuttall(const TSample &x)
{
	return 0.355768 - 0.487396 * cos(2.0 * M_PI * x) + 0.144232 * cos(4.0 * M_PI * x) - 0.012604 * cos(6.0 * M_PI * x);
}

template <class TSample>
inline TSample blackmannuttall(const TSample &x)
{
	return 0.3635819 - 0.4891775 * cos(2.0 * M_PI * x) + 0.1365995 * cos(4.0 * M_PI * x) - 0.0106411 * cos(6.0 * M_PI * x);
}

template <class TSample>
inline TSample blackmanharris(const TSample &x)
{
	return 0.35875 - 0.48829 * cos(2.0 * M_PI * x) + 0.14128 * cos(4.0 * M_PI * x) - 0.01168 * cos(6.0 * M_PI * x);
}

template <class TSample>
inline TSample flattop(const TSample &x)
{
	return 0.21557895 - 0.41663158 * cos(2.0 * M_PI * x) + 0.277263158 * cos(4.0 * M_PI * x) - 0.083578947 * cos(6.0 * M_PI * x) + 0.006947368 * cos(8.0 * M_PI * x);
}

template <class TSample>
inline TSample bartletthann(const TSample &x)
{
	return 0.62 - 0.48 * abs(x - 0.5) - 0.38 * cos(2.0 * M_PI * x);
}

template <class TSample>
inline TSample rectangular(const TSample &x)
{
    return 1.0;
}

#endif // WINDOW_FUNCTIONS_H_
