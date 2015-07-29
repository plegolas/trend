#ifndef NIKOLOV2_HPP
#define NIKOLOV2_HPP

#include <ctime>
#include <cmath>
#include <cfloat>
#include <list>
#include "signal.hpp"
#include "signalSource.hpp"
#include "params.hpp"

enum _position { pout, plong, pshort };


class nikolov2{
	public:
		vector<int> detect( signalSource source, vector<signal> rplus, vector<signal> rminus, vector<signal> rzero, 
						double gama, double theta, int detectionsLimit );

		double nikosProbability( signal s, vector<signal> rplus, vector<signal> rminus, double gama);

		int detection( double prob, double theta, int *counter, int detectionsLimit );

		float distToReference( signal s, signal r );

		float dist( signal s, signal t );

		double probClass( list<float> dist, double gama );
};
#endif
