#ifndef NIKOLOV_HPP
#define NIKOLOV_HPP

#include <ctime>
#include <cmath>
#include <cfloat>
#include <list>
#include "signal.hpp"
#include "signalSource.hpp"
#include "params.hpp"

vector<int> detect( signalSource source, vector<signal> rplus, vector<signal> rminus, 
//~ list< pair<time_t,signal> > detect( signalSource source, vector<signal> rplus, vector<signal> rminus, 
				double gama, double theta, int detectionsLimit );

float distToReference( signal s, signal r );

float dist( signal s, signal t );

double probClass( list<float> dist, double gama );

#endif
