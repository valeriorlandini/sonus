/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include <cmath>
#include <vector>

using namespace c74::min;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class halaphon_tilde : public object<halaphon_tilde>, public vector_operator<>
{
public:
	MIN_DESCRIPTION {"Sound rotator across an arbitrary number of speakers"};
	MIN_TAGS {"audio, routing, panning"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"quadpan~"};

	inlet<> in {this, "(signal) Input", "signal"};
	inlet<> in_speed {this, "(signal) Rotation speed (Hz)", "signal"};

    message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			if (static_cast<double>(args[0]) > 1.0)
            {
                inv_sample_rate_ = 1.0 / static_cast<double>(args[0]);
            }
            phasor_ = 0.0;
			return {};
		}
	};

	argument<int> outputs_arg
    {
        this,
        "outputs",
        "The number of output channels."
    };

	argument<number> speed_arg
    {
        this,
        "speed",
        "The initial rotation speed."
    };

    attribute<number, threadsafe::no, limit::clamp> width
	{
        this,
        "width",
        1.0,
		range { 0.0, 2.0 },
        title {"Panning width"},
		description {"Sets the width of the panning. A value of 0 means that each speaker plays alone, 1 means that the sound is equally distributed between two adjacent speakers, 2 means that the sound is always played by all speakers, albeit at different levels."},
        setter
        {
            MIN_FUNCTION
            {
                width_ = convert_width_(double(args[0]));
                return args;
            }
        }
    };

    halaphon_tilde(const atoms& args = {})
    {
        outputs_ = 2;

        if (!args.empty() && int(args[0]) > 1)
        {
            outputs_ = std::min(int(args[0]), 256);
        }

        gains_.assign(outputs_, 0.0);

        if (args.size() > 1)
        {
            frequency_ = double(args[1]);
        }

	    for (auto i = 0; i < outputs_; ++i)
        {
            m_outlets.push_back(std::make_unique<outlet<>>(this, "(signal) Output at speaker" + std::to_string(i+1), "signal"));
        }

        width_ = convert_width_(width);
    }

    message<> m_number
	{
		this,
		"number",
		"Frequency",
        MIN_FUNCTION
		{
            if (inlet == 1)
            {
                frequency_ = double(args[0]);
            }

			return {};
		}
    }; 

	void operator()(audio_bundle input, audio_bundle output)
    {   
        for (auto i = 0; i < input.frame_count(); ++i)
        {
            if (in_speed.has_signal_connection())
            {
                frequency_ = input.samples(1)[i];
            }

            update_gains_();
            
            for (auto ch = 0; ch < outputs_; ch++)
            {
                output.samples(ch)[i] = input.samples(0)[i] * gains_[ch];
            }
	    }
    }

private:
    int outputs_;
    std::vector<std::unique_ptr<outlet<>>> m_outlets;
    std::vector<double> gains_;
    double frequency_ = 0.0;
    double inv_sample_rate_ = 1.0 / 44100.0;
    double width_ = 1.0;
    double phasor_ = 0.0;
    const double double_pi_ = static_cast<double>(M_PI * 2.0);

    inline double convert_width_(double x)
    {
        double n = static_cast<double>(outputs_);

        return (x * x - (n + 1) * x + 2 * n) / 2;
    }

    inline void update_gains_()
    {
        phasor_ -= frequency_ * inv_sample_rate_;
        while (phasor_ > 1.0)
        {
            phasor_ -= 1.0; 
        }
        while (phasor_ < 0.0)
        {
            phasor_ += 1.0;
        }

        for (auto g = 0; g < outputs_; g++)
        {
            gains_[g] = phasor_ + static_cast<double>(g) / static_cast<double>(outputs_);
            gains_[g] = std::fmod(gains_[g], 1.0);
            gains_[g] *= width_;
            if (gains_[g] > 1.0)
            {
                gains_[g] = 1.0;
            }
            gains_[g] *= 0.5;
            gains_[g] -= 0.25;
            gains_[g] *= double_pi_;
            gains_[g] = std::cos(gains_[g]);
        }
    }
};


MIN_EXTERNAL(halaphon_tilde);
