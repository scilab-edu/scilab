 function [X]=prob(A1, A2)
 // Generated by lmitool on

   Mbound = 1e3;
   abstol = 1e-10;
   nu = 10;
   maxiters = 100;
   reltol = 1e-10;
   options=[Mbound,abstol,nu,maxiters,reltol];

 ///////////DEFINE INITIAL GUESS AND PRELIMINARY CALCULATIONS BELOW
 X_init=zeros(A1)
 ///////////

 XLIST0=list(X_init)
 XLIST=lmisolver(XLIST0,prob_eval,options)
 [X]=XLIST(:)
endfunction
 /////////////////EVALUATION FUNCTION////////////////////////////

 function [LME,LMI,OBJ]=prob_eval(XLIST)
 [X]=XLIST(1)

 /////////////////DEFINE LME, LMI and OBJ BELOW
 LME=X-diag(diag(X))
 LMI=list(-(A1'*X+X*A1+Q1), -(A2'*X+X*A2+Q2))
 OBJ=-sum(diag(X))
endfunction