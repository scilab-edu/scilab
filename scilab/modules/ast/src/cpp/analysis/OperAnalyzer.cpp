/*
 *  Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 *  Copyright (C) 2015 - Scilab Enterprises - Calixte DENIZET
 *
 *  This file must be used under the terms of the CeCILL.
 *  This source file is licensed as described in the file COPYING, which
 *  you should have received as part of this distribution.  The terms
 *  are also available at
 *  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */

#include "AnalysisVisitor.hxx"
#include "analyzers/OperAnalyzer.hxx"
#include "allexp.hxx"
#include "allvar.hxx"
#include "alltypes.hxx"

namespace analysis
{
    bool OperAnalyzer::analyze(AnalysisVisitor & visitor, ast::Exp & e)
    {
        ast::OpExp & oe = static_cast<ast::OpExp &>(e);
        const ast::OpExp::Oper oper = oe.getOper();
        if (oper == ast::OpExp::plus || oper == ast::OpExp::minus || oper == ast::OpExp::times)
        {
            if (ast::MemfillExp * mfe = analyzeMemfill(visitor, oe))
	    {
		mfe->setVerbose(e.isVerbose());
		e.replace(mfe);

		return true;
	    }
        }

	/*if (ast::ExtendedOpExp * eoe = analyzeMemfill(visitor, oe))
	{
	    eoe->setVerbose(e.isVerbose());
	    e.replace(eoe);
	    
	    return true;
	    }*/
	
	return false;
    }

    /*ast::ExtendedOpExp * OperAnalyzer::analyzeTransposedArgs(ast::OpExp & oe)
    {
	ast::Exp & L = oe.getLeft();
        ast::Exp & R = oe.getRight();
	ast::ExtendedOpExp::OP Lop, Rop;
	ast::Exp * Le = &L;
	ast::Exp * Re = &R;

	if (L.isTransposeExp())
	{
	    ast::TransposeExp & te = static_cast<ast::TransposeExp &>(L);
	    if (te.getConjugate() == ast::TransposeExp::_Conjugate_)
	    {
		Lop = ast::ExtendedOpExp::ADJOINT;
	    }
	    else
	    {
		Lop = ast::ExtendedOpExp::TRANSP;
	    }
	    Le = &te.getExp();
	}

	if (R.isTransposeExp())
	{
	    ast::TransposeExp & te = static_cast<ast::TransposeExp &>(R);
	    if (te.getConjugate() == ast::TransposeExp::_Conjugate_)
	    {
		Rop = ast::ExtendedOpExp::ADJOINT;
	    }
	    else
	    {
		Rop = ast::ExtendedOpExp::TRANSP;
	    }
	    Re = &te.getExp();
	}

	if (Lop != ast::ExtendedOpExp::NONE || Rop != ast::ExtendedOpExp::NONE)
	{
	    return new ast::ExtendedOpExp(oe.getLocation(), *Le, Lop, oe.getOper(), *Re, Rop);
	}

	return nullptr;
	}*/
    
    ast::MemfillExp * OperAnalyzer::analyzeMemfill(AnalysisVisitor & visitor, ast::OpExp & oe)
    {
        const ast::OpExp::Oper oper = oe.getOper();
        ast::Exp & L = oe.getLeft();
        ast::Exp & R = oe.getRight();

        ast::Exp * constant = nullptr;
        ast::exps_t args;
        double value;
        bool callAtLeft;

        // We try to match something like A +* ones(...) or A +* zeros(...)
        if (L.isCallExp())
        {
            ast::CallExp & ce = static_cast<ast::CallExp &>(L);
            if (ce.getName().isSimpleVar())
            {
                const std::wstring & name = static_cast<ast::SimpleVar &>(ce.getName()).getSymbol().getName();
                if (name == L"ones")
                {
                    value = 1;
                    args = ce.getArgs();
                    constant = &R;
                    callAtLeft = true;
                }
                else if (name == L"zeros")
                {
                    value = 0;
                    args = ce.getArgs();
                    constant = &R;
                    callAtLeft = true;
                }
            }
        }

        if (!constant)
        {
            if (R.isCallExp())
            {
                ast::CallExp & ce = static_cast<ast::CallExp &>(R);
                if (ce.getName().isSimpleVar())
                {
                    const std::wstring & name = static_cast<ast::SimpleVar &>(ce.getName()).getSymbol().getName();
                    if (name == L"ones")
                    {
                        value = 1;
                        args = ce.getArgs();
                        constant = &L;
                        callAtLeft = false;
                    }
                    else if (name == L"zeros")
                    {
                        value = 0;
                        args = ce.getArgs();
                        constant = &L;
                        callAtLeft = false;
                    }
                }
            }
        }

        if (constant)
        {
	    ast::exps_t cloneArgs;
	    cloneArgs.reserve(args.size());
	    for (auto arg : args)
	    {
		cloneArgs.push_back(arg->clone());
	    }
	    
            switch (oper)
            {
            case ast::OpExp::plus :
            {
                // plus is commutative so callAtLeft is ignored
                Result & res = constant->getDecorator().getResult();

                if (res.getType().ismatrix() && res.getType().isscalar())
                {
                    // we have something like x + ones(...) => it is a fill with x+1
                    const Location & loc = oe.getLocation();
                    double x;
                    if (res.getConstant().getDblValue(x))
                    {
                        return new ast::MemfillExp(loc, *new ast::DoubleExp(loc, new types::Double(x + value)), cloneArgs);
                    }
                    else
                    {
                        return new ast::MemfillExp(loc, *new ast::OpExp(loc, *constant->clone(), ast::OpExp::plus, *new ast::DoubleExp(loc, new types::Double(value))), cloneArgs);
                    }
                }
                break;
            }
            case ast::OpExp::minus :
            {
                Result & res = constant->getDecorator().getResult();

                if (res.getType().ismatrix() && res.getType().isscalar())
                {
                    // we have something like x - ones(...) => it is a fill with x-1
                    const Location & loc = oe.getLocation();
                    double x;
                    if (res.getConstant().getDblValue(x))
                    {
                        return new ast::MemfillExp(loc, *new ast::DoubleExp(loc, new types::Double(callAtLeft ? value - x : x - value)), cloneArgs);
                    }
                    else if (callAtLeft)
                    {
                        return new ast::MemfillExp(loc, *new ast::OpExp(loc, *new ast::DoubleExp(loc, new types::Double(value)), ast::OpExp::minus, *constant->clone()), cloneArgs);
                    }
                    else
                    {
                        return new ast::MemfillExp(loc, *new ast::OpExp(loc, *constant->clone(), ast::OpExp::minus, *new ast::DoubleExp(loc, new types::Double(value))), cloneArgs);
                    }
                }

                break;
            }
            case ast::OpExp::times :
            {
                // times is commutative so callAtLeft is ignored
                Result & res = constant->getDecorator().getResult();

                if (res.getType().ismatrix() && res.getType().isscalar())
                {
                    // we have something like x + ones(...) => it is a fill with x+1
                    const Location & loc = oe.getLocation();
                    double x;
                    if (res.getConstant().getDblValue(x))
                    {
                        return new ast::MemfillExp(loc, *new ast::DoubleExp(loc, new types::Double(x * value)), cloneArgs);
                    }
                    else if (value == 0)
                    {
                        return new ast::MemfillExp(loc, *new ast::DoubleExp(loc, new types::Double(0)), cloneArgs);
                    }
                    else
                    {
                        return new ast::MemfillExp(loc, *constant->clone(), cloneArgs);
                    }
                }
                break;
            }
            }
        }

        return nullptr;
    }
}