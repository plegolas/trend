#include "mm.hpp"

float mm::average(){
	float av = 0;
	for( int i = 0 ; i < values.size(); i++ ){
		av += values[i];
	}
	return av / (float)values.size();
}

void mm::init( std::vector<float> values_ ){
	period = values_.size();
	values.assign( values_.begin(), values_.end() );
}

void mm::add( float value ){
	int i;
	for( i = 1 ; i < values.size(); i++ ){
		values[i-1] = values[i];
	}
	values[i] = value;
}


int mm::get_period(){
	return period;
}
