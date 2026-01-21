/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2026 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "ecaosc.h"

using namespace c74::min;
using namespace soutel;

class ecaosc_tilde : public object<ecaosc_tilde>, public sample_operator<1, 1>
{
public:
	MIN_DESCRIPTION {"Wavetable oscillator with user defined table"};
	MIN_TAGS {"oscillators"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.rsosc~, sonus.vaosc~, sonus.ca"};

	inlet<>  in {this, "(signal/float) Frequency"};
	outlet<> out {this, "(signal) Output", "signal"};

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			osc_.set_sample_rate(args[0]);
			return {};
		}
	};

	argument<number> frequency_arg
	{
		this,
		"frequency",
		"Oscillator frequency.",
        MIN_ARGUMENT_FUNCTION
		{
            osc_.set_frequency((double)arg);
        }
    };

	attribute<int, threadsafe::no, limit::clamp> rule
	{
        this,
        "rule",
        110,
		range { 0, 255 },
        title {"Rule"},
        description {"Rule number."},
		setter
		{
			MIN_FUNCTION
			{
				osc_.set_rule_number(static_cast<uint8_t>(int(args[0])));
				return args;
			}
		}
    };

	attribute<std::vector<int>> cells
	{
		this,
		"cells",
		{0, 0, 0, 1, 1, 0, 0, 0},
		title {"Cells"},
		description {"Initial cells statuses."},
		setter
		{
			MIN_FUNCTION
			{
				if (args.size() > 0)
				{
					osc_.set_cells_number(args.size(), true);
					for (int c = 0; c < args.size(); c++)
					{
						if (int(args[c]) > 0)
						{
							osc_.set_cell_status(true, c);
						}
					}
					
					return args;
				}

				return cells;				
			}

		}
	};

	message<> random
    {
        this,
        "random",
        "Randomize cell statuses, optionally specifying alive chance (0-1)",
        MIN_FUNCTION
        {
            if (args.size() < 1)
            {
                osc_.randomize_status();
            }
			else
			{
				osc_.randomize_status(double(args[0]));
			}

			std::vector<bool> new_statuses_b = osc_.get_cells();
			for (int s = 0; s < new_statuses_b.size(); s++)
			{
				cells[s] = (int)new_statuses_b.at(s);
			}
			cells.set(cells);

            return {};
        }
    };    

	message<> m_number
	{
		this,
		"number",
		"Oscillator frequency",
        MIN_FUNCTION
		{
			osc_.set_frequency((double)args[0]);
			return {};
		}
    };

	sample operator()(sample x)
    {	
		double output = osc_.run();
		
		if (in.has_signal_connection())
		{
			osc_.set_frequency(x);
		}

		return output;
	}

	private:
	ECAOsc<double> osc_{44100.0, 1.0, 8u, 110, true};
};

MIN_EXTERNAL(ecaosc_tilde);
