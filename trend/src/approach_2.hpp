#ifndef APPROACH_2_CPP
#define APPROACH_2_CPP

#include <vector>
#include <iostream>
#include "signalSource.hpp"
#include "signal.hpp"
#include "params.hpp"
#include "mytools.hpp"
#include "mm.hpp"

class approach_2{
	public:
		void build_sets( signalSource source, vector<signal> *rplus, vector<signal> *rminus );
		vector<int> decision_ma( vector<float> vsource, int period1, int period2 );
};

#endif

