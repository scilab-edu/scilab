// Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
// Copyright (C) INRIA - Vincent COUVERT
// Copyright (C) 2012 - 2016 - Scilab Enterprises
// Copyright (C) 2020 - Samuel GOUGEON
//
// This file is hereby licensed under the terms of the GNU GPL v2.0,
// pursuant to article 5.3.4 of the CeCILL v.2.1.
// This file was originally licensed under the terms of the CeCILL v2.1,
// and continues to be available under such terms.
// For more information, see the COPYING file which you should have received
// along with this program.

function [txt, space] = format_txt(txt, instr, bprettyprintformat, nextinstr)
    // Add EOL and returns spaces to have good indentation (called by tree2code)
    // Inputs:
    // - txt: code to format
    // - instr: instruction to evaluate as a possible EOL
    // - bprettyprintformat = %T -> code generated is formatted 
    //                        %F -> code generated has same format as original code
    // - nextinstr: next instruction after instr (format EOL between instructions)
    // Outputs:
    // - txt: updated txt
    // - space: indentation margin for control instructions

    rhs = argn(2)

    // Indentation and EOL in control clauses
    if rhs==3 then
        if instr<>list("EOL") & bprettyprintformat then
            txt = cat_code(txt,"")
            space = "  "
        elseif instr<>list("EOL") then
            space = " "
        else
            space = "  "
        end
        // EOL between instructions
    else
        if instr <> list("EOL")
            if bprettyprintformat & nextinstr<>list("EOL")
                if nextinstr(1)(1) <> "comment"  // http://bugzilla.scilab.org/16586
                    txt = cat_code(txt,"")
                else
                    txt($) = txt($) + "  "
                end
            elseif ~bprettyprintformat & nextinstr(1)(1)=="comment"
                txt($) = txt($) + "  "
            end
        end
    end
endfunction
