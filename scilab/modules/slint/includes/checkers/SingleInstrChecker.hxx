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

#ifndef __SLINT_SINGLE_INSTR_CHECKER_HXX__
#define __SLINT_SINGLE_INSTR_CHECKER_HXX__

#include "SLintChecker.hxx"

namespace slint
{

/**
 * Check if there is an instruction by line
 */
class SingleInstrChecker : public SLintChecker
{

public:

    SingleInstrChecker(const std::wstring & checkerId) : SLintChecker(checkerId) { }
    ~SingleInstrChecker() { }

    void preCheckNode(const ast::Exp & e, SLintContext & context, SLintResult & result);
    void postCheckNode(const ast::Exp & e, SLintContext & context, SLintResult & result);
    const std::string getName() const;

    virtual const std::vector<ast::Exp::ExpType> getAstNodes() const
    {
        return { ast::Exp::SEQEXP };
    }
};

} // namespace slint

#endif // __SLINT_SINGLE_INSTR_CHECKER_HXX__