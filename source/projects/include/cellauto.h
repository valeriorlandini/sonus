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

#ifndef CELLAUTO_H_
#define CELLAUTO_H_

#include <array>
#include <cstdlib>
#include <ctime>
#include <vector>

template <class TSample>
class CellAuto
{
    public:

    CellAuto(unsigned int rows = 8, unsigned int cols = 8)
    {
        rows > 0 ? rows_ = rows : rows_ = 8;
        cols > 0 ? cols_ = cols : cols_ = 8;

        resize_matrix(rows_, cols_);

        // Initialize Conway's rules
        for (unsigned int n = 0; n < 9; n++)
        {
            set_rule(true, n, false); 
            set_rule(false, n, false);            
        }
        set_rule(true, 2, true);
        set_rule(true, 3, true);
        set_rule(false, 2, true);

        srand(time(0));
    }

    bool resize_matrix(const unsigned int &rows, const unsigned int &cols)
    {
        if ((rows * cols) != 0)
        {
            rows_ = rows;
            cols_ = cols;
            states_.clear();
            std::vector<bool> col(cols_, false);
            states_.resize(rows_, col);

            return true;
        }

        return false;
    }

    bool set_cell_state(const unsigned int &row, const unsigned int &col,
                        const bool &alive)
    {
        if (row < rows_ && col < cols_)
        {
            states_.at(row).at(col) = alive;

            return true;
        }

        return false;
    }

    bool invert_cell_state(const unsigned int &row, const unsigned int &col)
    {
        if (row < rows_ && col < cols_)
        {
            states_.at(row).at(col) = !(states_.at(row).at(col));
            
            return true;
        }

        return false;       
    }

    bool set_rule(const bool &is_survive, const unsigned int &neighbors, const bool &state)
    {
        if (neighbors < 9)
        {
            if (is_survive)
            {
                survive_[neighbors] = state;
            }
            else
            {
                born_[neighbors] = state;
            }

            return true;
        }

        return false;
    }

    inline unsigned int get_rows()
    {
        return rows_;
    }
    
    inline unsigned int get_cols()
    {
        return cols_;
    }

    std::vector<bool> get_rule(const unsigned int &neighbors)
    {
        std::vector<bool> born_surv_rule(2, false);
        
        if (neighbors < 9)
        {
            born_surv_rule = {born_[neighbors], survive_[neighbors]};
        }
        else
        {
            born_surv_rule = {born_[8], survive_[8]};
        }

        return born_surv_rule;
    }

    bool get_cell_state(const unsigned int &row, const unsigned int &col)
    {
        if (row < rows_ && col < cols_)
        {
            return states_.at(row).at(col);
        }

        return false;
    }


    unsigned int get_alive_neighbors(const unsigned int &row, const unsigned int &col)
    {
        unsigned int alive_neighbors = 0;
        int current_row, current_col;

        for (int nr = -1; nr < 2; nr++)
        {
            current_row = (int)row + nr;

            // Wrap matrix if necessary
            if (current_row < 0)
            {
                current_row = rows_ - 1;
            }
            if (current_row >= rows_)
            {
                current_row = 0;
            }

            for (int nc = -1; nc < 2; nc++)
            {
                current_col = (int)col + nc;
            
                // Wrap matrix if necessary
                if (current_col < 0)
                {
                    current_col = cols_ - 1;
                }
                if (current_col >= cols_)
                {
                    current_col = 0;
                }

                if (get_cell_state((unsigned int)current_row, (unsigned int)current_col))
                {
                    ++alive_neighbors;
                }
            }
        }

        // Exclude requested cell from neighbor count
        if (get_cell_state(row, col))
        {
            --alive_neighbors;
        }

        return alive_neighbors;
    }

    inline void update_matrix()
    {
        std::vector<std::vector<bool>> new_states_ = states_;

        for (unsigned int r = 0; r < rows_; r++)
        {
            for (unsigned int c = 0; c < cols_; c++)
            {
                // Get number of living neighbors
                int neighbors = get_alive_neighbors(r, c);

                // Get current state
                bool current_state = get_cell_state(r, c);

                // The cell is alive
                if (current_state)
                {
                    new_states_.at(r).at(c) = survive_[neighbors];
                }
                // The cell is dead
                else
                {
                    new_states_.at(r).at(c) = born_[neighbors];
                }
            }
        }

        states_ = new_states_;
    }

    void fill_random_matrix(const TSample &alive_chance = 0.5)
    {
        for (unsigned int r = 0; r < rows_; r++)
        {
            for (unsigned int c = 0; c < cols_; c++)
            {
                TSample number = (TSample)rand() / (TSample)RAND_MAX;
                set_cell_state(r, c, (bool)(number < alive_chance));
            }
        }
    }

    private:
    std::vector<std::vector<bool>> states_;
    unsigned int rows_;
    unsigned int cols_;
    std::array<bool, 9> survive_{false};
    std::array<bool, 9> born_{false};

};

#endif // CELLAUTO_H_
