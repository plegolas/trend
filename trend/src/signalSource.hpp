#ifndef SIGNALSOURCE_HPP
#define SIGNALSOURCE_HPP

#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <ctime>
#include <iterator>
#include "signal.hpp"
#include "mytools.hpp"

typedef list< pair<time_t,float> > SOURCETYPE;
typedef list< pair<time_t,float> >::iterator SOURCEIT;

using namespace std;

class signalSource{
	//time series
	SOURCETYPE serie;
	//online time series pointer simulator
	SOURCEIT itLastValue, itFirstValue;
	
	//used in the configure function
	bool my_compare (const std::pair<time_t,float>& first, 
					const std::pair<time_t,float>& second){
		return ( first.first < second.first );
	}
	
	public:
		signalSource(){ serie.clear(); }
		~signalSource(){ serie.clear(); }
		
		int lenght(){ return serie.size(); }
		
		void clear();
		
		bool hasData();
		
		void print();
		
		void add( time_t date, float value );
		
		void finish();
		
		void configure( int nPos );
		
		float at( int pos );
		
		time_t get_time();
		
		signal lastBlock();
		
		signal nextBlock();
		
		signal nextBlock( int skip_size );
		
		pair<time_t,float> lastValue();
		
		pair<time_t,float> nextValue();
};

#endif
