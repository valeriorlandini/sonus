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
        if (t > 0.0 && t < 0.1)
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
        TSample x_1 = -1.0 * y_ - z_;
        TSample y_1 = x_ + (a_ * y_);
        TSample z_1 = b_ + z_ * (x_ - c_);

        x_ += t_ * x_1;
        y_ += t_ * y_1;
        z_ += t_ * z_1;
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