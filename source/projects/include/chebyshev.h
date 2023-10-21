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

#ifndef CHEBYSHEV_H_
#define CHEBYSHEV_H_

#include <cmath>

template <class TSample>
inline TSample chebyshev(const TSample &input, const unsigned int &order)
{
    TSample output = 0.0;

    switch (order)
    	{
    		case 0:
    		    output = 1.0;
    		    break;
    		case 1:
    		    output = input;
    		    break;
    		case 2:
    		    output = 2.0 * pow(input, 2.0) - 1.0;
    		    break;
    		case 3:
    		    output = 4.0 * pow(input, 3.0) - 3.0 * input;
    		    break;
    		case 4:
    		    output = 8.0 * pow(input, 4.0) - 8.0 * pow(input, 2.0) + 1.0;
    		    break;
    		case 5:
    		    output = 16.0 * pow(input, 5.0) - 20.0 * pow(input, 3.0) + 5.0 * input;
    		    break;
    		case 6:
    		    output = 32.0 * pow(input, 6.0) - 48.0 * pow(input, 4.0) + 18.0 * pow(input, 2.0) - 1.0;
    		    break;
    		case 7:
    		    output = 64.0 * pow(input, 7.0) - 112.0 * pow(input, 5.0) + 56.0 * pow(input, 3.0) - 7.0 * input;
    		    break;
    		case 8:
    		    output = 128.0 * pow(input, 8.0) - 256.0 * pow(input, 6.0) + 160.0 * pow(input, 4.0) - 32.0 * pow(input, 2.0) + 1.0;
    		    break;
    		case 9:
    		    output = 256.0 * pow(input, 9.0) - 576.0 * pow(input, 7.0) + 432.0 * pow(input, 5.0) - 120.0 * pow(input, 3.0) + 9.0 * input;
    		    break;
    		case 10:
    		    output = 512.0 * pow(input, 10.0) - 1280.0 * pow(input, 8.0) + 1120.0 * pow(input, 6.0) - 400.0 * pow(input, 4.0) + 50.0 * pow(input, 2.0) - 1.0;
    		    break;
    	}

    return output;
}

#endif // CHEBYSHEV_H_
