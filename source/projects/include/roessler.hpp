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

#ifndef ROESSLER_HPP_
#define ROESSLER_HPP_

template <class TSample>
class Roessler
{
    public:
    Roessler(const TSample &a = 0.2, const TSample &b = 0.2,
           const TSample &c = 5.7, const TSample &x = 0.1,
           const TSample &y = 0.1, const TSample &z = 0.1,
           const TSample &t = 0.099)
    {
        a_ = a;
        b_ = b;
        c_ = c;
        x_ = x;
        y_ = y;
        z_ = z;
        if (!set_t(t))
        {
            set_t(0.099);
        }
    }

    void set_a(const TSample &a)
    {
        a_ = a;
    }

    void set_b(const TSample &b)
    {
        b_ = b;
    }

    void set_c(const TSample &c)
    {
        c_ = c;
    }

    void set_x(const TSample &x)
    {
        x_ = x;
    }

    void set_y(const TSample &y)
    {
        y_ = y;
    }

    void set_z(const TSample &z)
    {
        z_ = z;
    }

    bool set_t(const TSample &t)
    {
        if (t >= 0.0 && t < 0.1)
        {
            t_ = t;

            return true;
        }

        return false;
    }

    TSample get_a()
    {
        return a_;
    }
    
    TSample get_b()
    {
        return b_;
    }
    
    TSample get_c()
    {
        return c_;
    }
    
    TSample get_x()
    {
        return x_;
    }
    
    TSample get_y()
    {
        return y_;
    }
    
    TSample get_z()
    {
        return z_;
    }

    inline void step()
    {
        if (t_ > 0.0)
        {
            TSample x_1 = (-1.0 * y_) - z_;
            TSample y_1 = x_ + (a_ * y_);
            TSample z_1 = b_ + (z_ * (x_ - c_));

            x_ += t_ * x_1;
            y_ += t_ * y_1;
            z_ += t_ * z_1;

            if (isnan(x_) || isnan(y_) || isnan(z_))
            {
                x_ = 0.1;
                y_ = 0.1;
                z_ = 0.1;
            }
        }
    }

    inline void step(TSample &x, TSample &y, TSample &z)
    {
        step();

        x = x_;
        y = y_;
        z = z_;
    }

    private:
    TSample a_, b_, c_;
    TSample x_, y_, z_;
    TSample t_;
};

#endif // ROESSLER_HPP_