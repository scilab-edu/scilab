// Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
// Copyright (C)  2016 - INRIA - Serge Steer
//
// This file is licensed under the terms of the GNU GPL v2.0,
// pursuant to article 5.3.4 of the CeCILL v.2.1.
// For more information, see the COPYING file which you should have received
// along with this program.
function r=%zpk_e(varargin)
    r=varargin($)
    r.K=r.K(varargin(1:$-1))
    if r.K==[] then r=[];return;end
    r.Z=r.Z(varargin(1:$-1))
    r.P=r.P(varargin(1:$-1))
endfunction