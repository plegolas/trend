#ifndef NIKOLOV1_HPP
#define NIKOLOV1_HPP

#include <ctime>
#include <cmath>
#include <cfloat>
#include <list>
#include "signal.hpp"
#include "signalSource.hpp"
#include "params.hpp"

class nikolov1{
	public:
		
		vector<int> detect( signalSource source, vector<signal> rplus, vector<signal> rminus, vector<signal> rzero, 
						double gama, double theta, int detectionsLimit );
						
		vector<int> detect( signalSource source, vector<signal> rplus, vector<signal> rminus, 
						double gama, double theta, int detectionsLimit );

		float distToReference( signal s, signal r );

		float dist( signal s, signal t );

		double probClass( list<float> dist, double gama );
};
#endif
