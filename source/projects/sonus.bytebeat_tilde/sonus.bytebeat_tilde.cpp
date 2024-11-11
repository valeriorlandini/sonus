/// @file
///	@ingroup 	sonus.*
///	@copyright	Copyright 2023-2024 Valerio Orlandini. All rights reserved.
///	@license	Use of this source code is governed by the MIT License found in the License.md file.

#include "parser/mpParser.h"
#include "c74_min.h"
#include <cstdint>
#include <string>
#include "biquad.h"

using namespace c74::min;
using namespace mup;
using namespace soutel;

class OprtMod : public IOprtBin    
{
public:
    OprtMod() 
        :IOprtBin(_T("%"), (int)prMUL_DIV, oaLEFT) 
    {}

    virtual void Eval(ptr_val_type &ret, const ptr_val_type *a_pArg, int)
    {
        const IValue *arg1 = a_pArg[0].Get();
        const IValue *arg2 = a_pArg[1].Get();

        if (arg2->GetInteger() != 0)
        {
            *ret = arg1->GetInteger() % arg2->GetInteger();
        }
        else
        {
            *ret = arg2->GetInteger();
        }
    }

    virtual const char_type* GetDesc() const
    { 
        return _T("x%y - Modulo operator"); 
    }
  
    virtual IToken* Clone() const
    { 
        return new OprtMod(*this); 
    }
};

class OprtDiv : public IOprtBin    
{
public:
    OprtDiv() 
        :IOprtBin(_T("/"), (int)prMUL_DIV, oaLEFT) 
    {}

    virtual void Eval(ptr_val_type &ret, const ptr_val_type *a_pArg, int)
    {
        const IValue *arg1 = a_pArg[0].Get();
        const IValue *arg2 = a_pArg[1].Get();

        if (arg2->GetInteger() != 0)
        {
            *ret = arg1->GetInteger() / arg2->GetInteger();
        }
        else
        {
            *ret = arg2->GetInteger();
        }
    }

    virtual const char_type* GetDesc() const
    { 
        return _T("x/y - Integer division"); 
    }
  
    virtual IToken* Clone() const
    { 
        return new OprtDiv(*this); 
    }
};

class OprtXor : public IOprtBin    
{
public:
    OprtXor() 
        :IOprtBin(_T("^"), (int)prBIT_OR, oaLEFT) 
    {}

    virtual void Eval(ptr_val_type &ret, const ptr_val_type *a_pArg, int)
    {
        const IValue *arg1 = a_pArg[0].Get();
        const IValue *arg2 = a_pArg[1].Get();

        *ret = arg1->GetInteger() ^ arg2->GetInteger(); 
    }

    virtual const char_type* GetDesc() const
    { 
        return _T("x^y - Xor bitwise operator"); 
    }
  
    virtual IToken* Clone() const
    { 
        return new OprtXor(*this); 
    }
};

class bytebeat_tilde : public object<bytebeat_tilde>, public sample_operator<0, 1>
{
public:
	MIN_DESCRIPTION {"Bytebeat music parser and generator"};
	MIN_TAGS {"algorithmic, generator"};
	MIN_AUTHOR {"Valerio Orlandini"};
	MIN_RELATED {"sonus.byteplay~"};

	inlet<>  in {this, "(symbol) Formula"};
	inlet<>  in_sr {this, "(int) Sample rate reduction factor (1-10)"};
	outlet<> out {this, "(signal) Output", "signal"};

	bytebeat_tilde()
	{
		t_ = (MUP_INT_TYPE)0;
		p.DefineVar("t", Variable(&t_));
		p.DefineOprt(new OprtMod);
		p.RemoveOprt("/");
		p.DefineOprt(new OprtDiv);
		p.RemoveOprt("^");
		p.DefineOprt(new OprtXor);
		p.SetExpr("0");
		try
		{
			p.Eval();
		}
		catch (mup::ParserError &e)
		{
			cout << e.GetMsg() << endl;
		}
	}

	message<> dspsetup
	{
		this,
		"dspsetup", 
		MIN_FUNCTION
		{
			lfo_lowpass_.set_sample_rate((double)args[0]);
			lfo_lowpass_.set_cutoff(70.0);
			return {};
		}
	};

	attribute<int, threadsafe::no, limit::clamp> sr_red
	{
        this,
        "red",
        4,
		range { 1, 10 },
        title {"SR Reduction"},
        description {"Factor for sample rate reduction. The lower it is, the higher the pitch of the sound is."},
		setter
		{
			MIN_FUNCTION
			{
				if (bool(lfo))
				{
					update_factor_ = int(args[0]) * 50;
				}
				else
				{
					update_factor_ = int(args[0]);
				}
				return args;
			}
		}
    };

	attribute<bool> lfo
	{
        this,
        "lfo",
        false,
        title {"LFO Mode"},
        description {"When activated, the frequency is reduced by 50x and the output is lowpassed, making it suitable for modulations."},
		setter
		{
			MIN_FUNCTION
			{
				if (bool(args[0]))
				{
					update_factor_ = int(sr_red) * 50;
					lfo_lowpass_.clear();
				}
				else
				{
					update_factor_ = int(sr_red);
				}
				return args;
			}
		}
    };

	message<> eval
	{
		this,
		"eval",
		"Formula to be evaluated. Variable name must be 't'",
		setter
		{
			MIN_FUNCTION
			{
				if (args.size() == 0)
				{
					return {};
				}
				std::string new_formula = "";
				for (auto a = 0; a < args.size(); a++)
				{
					new_formula += std::string(args[a]);
				}
				MUP_INT_TYPE old_t = t_.GetInteger();
				t_ = (MUP_INT_TYPE)0;
				p.SetExpr(new_formula);
				try
				{
					p.Eval();
				}
				catch (mup::ParserError &e)
				{
					cout << "Expression error: " << e.GetMsg() << endl;
					p.SetExpr(formula_);
					t_ = old_t;
					return {};
				}

				formula_ = new_formula;
				return {};
			}
		}
    };

	message<> m_number
	{
		this,
		"number",
		"In inlet 1, sample rate reduction factor",
        MIN_FUNCTION
		{
			if (inlet == 1)
			{
				sr_red = args[0];
			}

			return {};
		}
    };

	message<> reset
    {
        this,
        "reset",
        "Reset t variable to 0",
        MIN_FUNCTION
        {
            t_ = (MUP_INT_TYPE)0;

			return {};
		}
	};

	sample operator()()
    {
		++sample_count_;

		if (sample_count_ >= update_factor_)
		{
			t_ = t_.GetInteger() + (MUP_INT_TYPE)1;
			try
			{
				out_ = p.Eval().GetInteger();
			}
			catch (mup::ParserError &e)
			{
			}
			output_ = ((double)out_ - 127.5) / 127.5;
			sample_count_ = 0;
		}

		if (bool(lfo))
		{
			return { lfo_lowpass_.run(output_) };
		}
		
		return { output_ };
	}

	private:
	int sample_count_ = 0;
	std::string formula_;
	uint8_t out_ = 0;
	int update_factor_ = 4;
	double output_ = 0.0;
	Biquad<double> lfo_lowpass_;

	Value t_;
	ParserX p;
};

MIN_EXTERNAL(bytebeat_tilde);
