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

#ifndef DNN_H_
#define DNN_H_

#include <cmath>
#include <functional>
#include <string>
#include <vector>

namespace deepnn
{

template <typename T>
inline T relu(const T& x)
{
    return std::max(static_cast<T>(0.0), x);
}

template <typename T>
inline T leakyrelu(const T& x)
{
    return std::max(static_cast<T>(0.1) * x, x);
}

template <typename T>
inline T sigmoid(const T& x)
{
    return static_cast<T>(1.0) / (static_cast<T>(1.0) + std::exp(-static_cast<T>(1.0) * x));
}

template <typename T>
inline T tanh(const T& x)
{
    return (std::exp(x) - std::exp(-static_cast<T>(1.0) * x)) / (std::exp(x) + std::exp(-static_cast<T>(1.0) * x));
}

template <typename T>
inline T softplus(const T& x)
{
    return std::log(static_cast<T>(1.0) + std::exp(x));
}

template <typename T>
inline T selu(const T& x)
{
    if (x > static_cast<T>(0.0))
    {
        return static_cast<T>(1.050701) * x;
    }
    else
    {
        return static_cast<T>(1.7581) * (std::exp(x) - static_cast<T>(1.0));
    }
}

template <typename T>
inline T silu(const T& x)
{
    return x / (static_cast<T>(1.0) + std::exp(static_cast<T>(-1.0) * x));
}

template <typename T>
inline T binary(const T& x)
{
    return static_cast<T>(x > static_cast<T>(0.0));
}

template <typename T>
inline T gauss(const T& x)
{
    return std::exp(static_cast<T>(-1.0) * (x * x));
}

template <class T>
struct Neuron
{
    std::vector<T> weights;
    T bias;
    std::function<T(T)> activation;

    inline T run(std::vector<T> inputs)
    {
        if (inputs.size() < weights.size())
        {
            inputs.resize(weights.size(), static_cast<T>(0.0));
        }

        T output = bias;

        for (auto i = 0; i < weights.size(); i++)
        {
            output += inputs.at(i) * weights.at(i);
        }

        return activation(output);
    }

    void set_activation(const std::string &activationName)
    {
        if (activationName == "relu")
        {
            activation = &relu<T>;
        }
        else if (activationName == "leakyrelu")
        {
            activation = &leakyrelu<T>;
        }
        else if (activationName == "sigmoid")
        {
            activation = &sigmoid<T>;
        }
        else if (activationName == "tanh")
        {
            activation = &tanh<T>;
        }
        else if (activationName == "softplus")
        {
            activation = &softplus<T>;
        }
        else if (activationName == "selu")
        {
            activation = &selu<T>;
        }
        else if (activationName == "silu")
        {
            activation = &silu<T>;
        }
        else if (activationName == "binary")
        {
            activation = &binary<T>;
        }
        else if (activationName == "gauss")
        {
            activation = &gauss<T>;
        }
        else
        {
            activation = [](T x)
            {
                return x;
            };
        }
    }
};

template <typename T>
using Layer = std::vector<Neuron<T>>;

template <class T>
class DNN
{
public:
    void push_layer(const Layer<T> &layer)
    {
        layers_.push_back(layer);
    }

    void pop_layer()
    {
        layers_.pop_back();
    }

    void clear()
    {
        layers_.clear();
    }

    inline std::vector<T> run(const std::vector<T> input)
    {
        if (input.empty() || layers_.empty())
        {
            return {0.0};
        }

        std::vector<T> in = input;

        if (in.size() < layers_.at(0).size())
        {
            in.resize(layers_.at(0).size(), static_cast<T>(0.0));
        }

        std::vector<T> out;

        for (auto l = 0; l < layers_.size(); l++)
        {
            if (l > 0)
            {
                in = out;
                out.clear();
            }

            for (auto n = 0; n < layers_.at(l).size(); n++)
            {
                if (l == 0)
                {
                    in = input;
                }

                out.push_back(layers_.at(l).at(n).run(in));
            }
        }

        return out;
    }

private:
    std::vector<Layer<T>> layers_;
    std::vector<T> output_;
};

}

#endif // DNN_H_
