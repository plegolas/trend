#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <vector>
#include <iterator>
#include <iostream>
#include <cmath>
#include <fstream>
#include "mytools.hpp"
#include "params.hpp"

typedef std::vector<float> SIGNALTYPE;
typedef std::vector<float>::iterator SIGNALIT;

class signal{
	//série temporal
	SIGNALTYPE serie;
	
	bool transformed;
	
	void simple_returns();
	
	void accum_log();
	
	void nikos_signal();
	
	
	public:
		signal();
		signal( std::vector<float> s );
		~signal();
		
		bool is_transformed();
		void set_transformed( bool t );
		
		int lenght();
		
		void print();
		
		void print( string outfile );
		
		float at( int pos );
		
		void add( float value );
		
		void transform();
		
		void reduce( int size );
		
		void pop_front( int size );
		
		float trend_return( int trend_size );
		
		void baseline_normalization();
		void spike_normalization();
		void smoothing();
		void log_scale();
		
		signal subSignal( int begin, int size );
};

#endif
