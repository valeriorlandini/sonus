/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "cellauto.h"

using namespace c74::min;

class ca : public object<ca>
{
public:
	MIN_DESCRIPTION {"Cellular automata system"};
	MIN_TAGS {"ai, math"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.neuron, sonus.perceptron, sonus.ga"};

	inlet<>  in {this, "(bang) Evolve matrix"};
	outlet<> out {this, "(list) Output for matrixctrl", "list"};

	message<> m_bang
	{
		this,
		"bang",
		"Evolve matrix",
		MIN_FUNCTION
		{
			cellauto_.update_matrix();

			output_matrix_();

			return {};
		}
	};

	message<> born
    {
        this,
        "born",
        "Set the number of alive neighbors a dead cell should have to become alive",
        MIN_FUNCTION
        {
            if (!args.empty())
            {
				// Reset previous rules
				for (unsigned int n = 0; n < 9; n++)
				{
					cellauto_.set_rule(false, n, false);
				}

				for (unsigned int n = 0; n < args.size(); n++)
				{
					unsigned int nn = int(args.at(0));
					cellauto_.set_rule(false, nn, true);
				}
            }

			return {};
		}
	};

	message<> surv
    {
        this,
        "surv",
        "Set the number of alive neighbors an alive cell should to remain alive",
        MIN_FUNCTION
        {
            if (!args.empty())
            {
				// Reset previous rules
				for (unsigned int n = 0; n < 9; n++)
				{
					cellauto_.set_rule(true, n, false);
				}

				for (unsigned int n = 0; n < args.size(); n++)
				{
					unsigned int nn = int(args.at(0));
					cellauto_.set_rule(true, nn, true);
				}
            }

			return {};
		}
	};

	message<> rows
    {
        this,
        "rows",
        "Set the number of rows of the matrix",
        MIN_FUNCTION
        {
            if (!args.empty())
            {
                unsigned int new_rows = std::max(1, int(args.at(0)));
				cellauto_.resize_matrix(new_rows, cellauto_.get_cols());
            }

			return {};
		}
	};

	message<> cols
    {
        this,
        "cols",
        "Set the number of columns of the matrix",
        MIN_FUNCTION
        {
            if (!args.empty())
            {
                unsigned int new_cols = std::max(1, int(args.at(0)));
				cellauto_.resize_matrix(cellauto_.get_rows(), new_cols);
            }

			return {};
		}
	};

	message<> fill
    {
        this,
        "fill",
        "Randomly fill the the matrix, optionally specify the fill probability",
        MIN_FUNCTION
        {
			float probability = 0.5;

			if (!args.empty())
            {
				probability = float(args.at(0));
                probability = std::clamp(probability, 0.0f, 1.0f);
            }

			cellauto_.fill_random_matrix(probability);
			
			output_matrix_();

			return {};
		}
	};

	message<> muta
    {
        this,
        "muta",
        "Invert the state of n random chosen cells",
        MIN_FUNCTION
        {
			unsigned int cells = 1;

			if (!args.empty())
            {
				cells = int(args.at(0));
                cells = std::clamp(cells, 1u, cellauto_.get_rows() * cellauto_.get_cols());
            }

			for (unsigned int cell = 0; cell < cells; cell++)
			{
				unsigned int r = rand() % cellauto_.get_rows();
				unsigned int c = rand() % cellauto_.get_cols();
				cellauto_.set_cell_state(r, c, !(cellauto_.get_cell_state(r, c)));
			}
			
			output_matrix_();

			return {};
		}
	};

	message<> dump
    {
        this,
        "dump",
        "Output current matrix",
        MIN_FUNCTION
        {
			output_matrix_();
			
			return {};
		}
	};

	message<> cell
    {
        this,
        "cell",
        "Set a cell to the desired value",
        MIN_FUNCTION
        {
			if (args.size() >= 3)
            {
				unsigned int c = int(args.at(0));
				unsigned int r = int(args.at(1));
				bool s = bool(args.at(2));
				cellauto_.set_cell_state(r, c, s);
			}
			
			return {};
		}
	};

	private:
	CellAuto<float> cellauto_;
	void output_matrix_()
	{
		std::string cell_states = "set ";
		atoms coords;
		coords.reserve(1 + cellauto_.get_rows() * cellauto_.get_cols() * 3);
		coords.push_back("set");

    	for (unsigned int r = 0; r < cellauto_.get_rows(); r++)
    	{
        	for (unsigned int c = 0; c < cellauto_.get_cols(); c++)
        	{
				coords.push_back(c);
				coords.push_back(r);
				coords.push_back((unsigned int)cellauto_.get_cell_state(r, c));
			}	
    	}

		out.send(coords);
	}
};

MIN_EXTERNAL(ca);
