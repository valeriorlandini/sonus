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
