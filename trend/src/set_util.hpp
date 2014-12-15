#ifndef SET_UTIL_CPP
#define SET_UTIL_CPP

#include <vector>
#include <iostream>
#include <algorithm>
#include <cstring>
#include "signalSource.hpp"
#include "signal.hpp"
#include "params.hpp"
#include "mytools.hpp"

class set_util{
	public:
		void print(string filename, vector<signal> *r);
		void load( vector<signal> *r, string filename );
		void update( vector<signal> *r, string line );
		void init( vector<signal> *r, string line );
		void transform( vector<signal> *set );
};

#endif

