/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2025 Valerio Orlandini. All rights reserved.
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
    }

	void operator()(audio_bundle input, audio_bundle output)
    {
		std::vector<float> inputs_sample;
        
        for (auto i = 0; i < input.frame_count(); ++i)
        {
            update_gains_();
            
            for (auto ch = 0; ch < outputs_; ch++)
            {
                output.samples(ch)[i] = inputs.samples(0)[i] * gains_->at(ch);
            }
	    }
    }

private:
    int outputs_;
    std::vector<std::unique_ptr<outlet<>>> m_outlets;
    std::vector<double> gains_;
    double frequency_ = 0.0;
    double inv_sample_rate_ = 1.0 / 44100.0;
    double phasor_ = 0.0;
    const double double_pi_ = (double)(M_PI * 2.0);

    inline void update_gains_()
    {
        phasor_ += frequency_ * inv_sample_rate_;
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
            gains[g] = phasor_ + static_cast<double>(g) / static_cast<double>(outputs_);
            gains[g] = std::fmod(1.0, gains[g]);
            gains[g] *= static_cast<double>outputs_;
            if (gains[g] > 1.0)
            {
                gains[g] = 1.0;
            }
            gains[g] *= 0.5;
            gains[g] -= 0.25;
            gains[g] *= double_pi_;
            gains[g] = std::cos(gains[g]);
        }
    }
};


MIN_EXTERNAL(halaphon_tilde);
