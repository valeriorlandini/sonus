#ifndef LORENZ_HPP_
#define LORENZ_HPP_

#include <iostream>

template <class TSample>
class Lorenz
{
    public:
    Lorenz(const TSample &sigma = 10.0, const TSample &rho = 28.0,
           const TSample &beta = 8.0/3.0, const TSample &x = 0.01,
           const TSample &y = 0.0, const TSample &z = 0.0,
           const TSample &t = 0.099)
    {
        sigma_ = sigma;
        rho_ = rho;
        beta_ = beta;
        x_ = x;
        y_ = y;
        z_ = z;
        if (!set_t(t))
        {
            set_t(0.099);
        }
    }

    void set_sigma(const TSample &sigma)
    {
        sigma_ = sigma;
    }

    void set_rho(const TSample &rho)
    {
        rho_ = rho;
    }

    void set_beta(const TSample &beta)
    {
        beta_ = beta;
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

    
    TSample get_sigma()
    {
        return sigma_;
    }
    
    TSample get_rho()
    {
        return rho_;
    }
    
    TSample get_beta()
    {
        return beta_;
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
        TSample x_1 = sigma_ * (y_ - x_);
        TSample y_1 = x_ * (rho_ - z_) - y_;
        TSample z_1 = x_ * y_ - beta_ * z_;

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
    TSample sigma_, beta_, rho_;
    TSample x_, y_, z_;
    TSample t_;
};

#endif // LORENZ_HPP_