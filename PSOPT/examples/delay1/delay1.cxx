//////////////////////////////////////////////////////////////////////////
////////////////            delay1.cxx               /////////////////////
//////////////////////////////////////////////////////////////////////////
////////////////           PSOPT  Example             ////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////// Title:         Time delay problem                ////////////////
//////// Last modified: 09 January 2009                   ////////////////
//////// Reference:     Luus (2002)                       ////////////////
//////// (See PSOPT handbook for full reference)          ////////////////
//////////////////////////////////////////////////////////////////////////
////////     Copyright (c) Victor M. Becerra, 2009        ////////////////
//////////////////////////////////////////////////////////////////////////
//////// This is part of the PSOPT software library, which ///////////////
//////// is distributed under the terms of the GNU Lesser ////////////////
//////// General Public License (LGPL)                    ////////////////
//////////////////////////////////////////////////////////////////////////

#include "psopt.h"

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the end point (Mayer) cost function //////////
//////////////////////////////////////////////////////////////////////////

adouble endpoint_cost(adouble* initial_states, adouble* final_states,
                      adouble* parameters,adouble& t0, adouble& tf,
                      adouble* xad, int iphase, Workspace* workspace)
{
    adouble x3f = final_states[CINDEX(3)];
    return x3f;
}


//////////////////////////////////////////////////////////////////////////
///////////////////  Define the integrand (Lagrange) cost function  //////
//////////////////////////////////////////////////////////////////////////

adouble integrand_cost(adouble* states, adouble* controls,
                       adouble* parameters, adouble& time, adouble* xad,
                       int iphase, Workspace* workspace)
{
    return  0.0;
}

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the DAE's ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void dae(adouble* derivatives, adouble* path, adouble* states,
         adouble* controls, adouble* parameters, adouble& time,
         adouble* xad, int iphase, Workspace* workspace)
{

   adouble x1delayed, x2delayed;
   double  tau = 0.25;

   adouble x1 = states[CINDEX(1)];
   adouble x2 = states[CINDEX(2)];
   adouble x3 = states[CINDEX(3)];

   get_delayed_state( &x1delayed, 1, iphase, time, tau, xad, workspace);
   get_delayed_state( &x2delayed, 2, iphase, time, tau, xad, workspace);

   adouble u = controls[CINDEX(1)];

   derivatives[CINDEX(1)] = x2;
   derivatives[CINDEX(2)] = -10*x1-5*x2-2*x1delayed-x2delayed+u;
// [uncomment the line below for approximate solution]
//   derivatives[CINDEX(2)] = (-12*x1+(2*tau-6)*x2+u)/(1-tau);
   derivatives[CINDEX(3)] = 0.5*(10*x1*x1+x2*x2+u*u);
}

////////////////////////////////////////////////////////////////////////////
///////////////////  Define the events function ////////////////////////////
////////////////////////////////////////////////////////////////////////////

void events(adouble* e, adouble* initial_states, adouble* final_states,
            adouble* parameters,adouble& t0, adouble& tf, adouble* xad,
            int iphase, Workspace* workspace)
{
   adouble x10 = initial_states[CINDEX(1)];
   adouble x20 = initial_states[CINDEX(2)];
   adouble x30 = initial_states[CINDEX(3)];

   e[CINDEX(1)] = x10;
   e[CINDEX(2)] = x20;
   e[CINDEX(3)] = x30;

}


///////////////////////////////////////////////////////////////////////////
///////////////////  Define the phase linkages function ///////////////////
///////////////////////////////////////////////////////////////////////////

void linkages( adouble* linkages, adouble* xad, Workspace* workspace)
{
  // No linkages as this is a single phase problem
}


////////////////////////////////////////////////////////////////////////////
///////////////////  Define the main routine ///////////////////////////////
////////////////////////////////////////////////////////////////////////////

int main(void)
{

////////////////////////////////////////////////////////////////////////////
///////////////////  Declare key structures ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    Alg  algorithm;
    Sol  solution;
    Prob problem;

////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem name  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.name        		= "Time delay problem 1";
    problem.outfilename                 = "delay1.txt";

////////////////////////////////////////////////////////////////////////////
////////////  Define problem level constants & do level 1 setup ////////////
////////////////////////////////////////////////////////////////////////////


    problem.nphases   			= 1;
    problem.nlinkages                   = 0;

    psopt_level1_setup(problem);

/////////////////////////////////////////////////////////////////////////////
/////////   Define phase related information & do level 2 setup  ////////////
/////////////////////////////////////////////////////////////////////////////



    problem.phases(1).nstates   = 3;
    problem.phases(1).ncontrols = 1;
    problem.phases(1).nevents   = 3;
    problem.phases(1).npath     = 0;

    problem.phases(1).nodes     = "[30]";

    psopt_level2_setup(problem, algorithm);


////////////////////////////////////////////////////////////////////////////
///////////////////  Declare DMatrix objects to store results //////////////
////////////////////////////////////////////////////////////////////////////

    DMatrix x, u, t;
    DMatrix lambda, H;

////////////////////////////////////////////////////////////////////////////
///////////////////  Enter problem bounds information //////////////////////
////////////////////////////////////////////////////////////////////////////

    double x1L = -100.0;
    double x2L = -100.0;
    double x3L = -100.0;
    double x1U = 100.0;
    double x2U = 100.0;
    double x3U = 100.0;

    double uL = -100.0;
    double uU =  100.0;

    problem.phases(1).bounds.lower.states(1) = x1L;
    problem.phases(1).bounds.lower.states(2) = x2L;
    problem.phases(1).bounds.lower.states(3) = x3L;

    problem.phases(1).bounds.upper.states(1) = x1U;
    problem.phases(1).bounds.upper.states(2) = x2U;
    problem.phases(1).bounds.upper.states(3) = x3U;

    problem.phases(1).bounds.lower.controls(1) = uL;
    problem.phases(1).bounds.upper.controls(1) = uU;

    problem.phases(1).bounds.lower.events(1) = 1.0;
    problem.phases(1).bounds.lower.events(2) = 1.0;
    problem.phases(1).bounds.lower.events(3) = 0.0;


    problem.phases(1).bounds.upper.events(1) = 1.0;
    problem.phases(1).bounds.upper.events(2) = 1.0;
    problem.phases(1).bounds.upper.events(3) = 0.0;



    problem.phases(1).bounds.lower.StartTime    = 0.0;
    problem.phases(1).bounds.upper.StartTime    = 0.0;

    problem.phases(1).bounds.lower.EndTime      = 5.0;
    problem.phases(1).bounds.upper.EndTime      = 5.0;


////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem functions  ///////////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.integrand_cost 	= &integrand_cost;
    problem.endpoint_cost 	= &endpoint_cost;
    problem.dae             	= &dae;
    problem.events 		= &events;
    problem.linkages		= &linkages;

////////////////////////////////////////////////////////////////////////////
///////////////////  Define & register initial guess ///////////////////////
////////////////////////////////////////////////////////////////////////////


    DMatrix x0(3,60);

    x0(1,colon()) = linspace(1.0,1.0, 60);
    x0(2,colon()) = linspace(1.0,1.0, 60);
    x0(3,colon()) = linspace(0.0,0.0, 60);

    problem.phases(1).guess.controls       = zeros(1,60);
    problem.phases(1).guess.states         = x0;
    problem.phases(1).guess.time           = linspace(0.0,5.0, 60);

////////////////////////////////////////////////////////////////////////////
///////////////////  Enter algorithm options  //////////////////////////////
////////////////////////////////////////////////////////////////////////////

    algorithm.nlp_method                  = "IPOPT";
    algorithm.scaling                     = "automatic";
    algorithm.derivatives                 = "automatic";
    algorithm.nlp_iter_max                = 1000;
    algorithm.nlp_tolerance               = 1.e-6;
    algorithm.collocation_method          = "Hermite-Simpson";
//    algorithm.mesh_refinement             = "automatic";


////////////////////////////////////////////////////////////////////////////
///////////////////  Now call PSOPT to solve the problem   /////////////////
////////////////////////////////////////////////////////////////////////////

    psopt(solution, problem, algorithm);

////////////////////////////////////////////////////////////////////////////
///////////  Extract relevant variables from solution structure   //////////
////////////////////////////////////////////////////////////////////////////

    x = solution.get_states_in_phase(1);
    u = solution.get_controls_in_phase(1);
    t = solution.get_time_in_phase(1);

////////////////////////////////////////////////////////////////////////////
///////////  Save solution data to files if desired ////////////////////////
////////////////////////////////////////////////////////////////////////////

    x.Save("x.dat");
    u.Save("u.dat");
    t.Save("t.dat");

////////////////////////////////////////////////////////////////////////////
///////////  Plot some results if desired (requires gnuplot) ///////////////
////////////////////////////////////////////////////////////////////////////

    plot(t,x,problem.name, "time (s)", "states", "x1 x2 x3");
    plot(t,u,problem.name, "time (s)", "control", "u");
    plot(t,x,problem.name, "time (s)", "states", "x1 x2 x3",
                           "pdf", "delay1_states.pdf");
    plot(t,u,problem.name, "time (s)", "control", "u",
                           "pdf", "delay1_controls.pdf");

}



