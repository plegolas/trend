#include "mm.hpp"

float mm::average(){
	if( !is_full ){
		return 0;
	}
	float av = 0;
	for( int i = 0 ; i < values.size(); i++ ){
		av += values[i];
	}
	return av / (float)values.size();
}

void mm::set_size( int size ){
	values.resize( size, 0 );
	is_full = false;
	oldest_value = 0;
}

void mm::init( std::vector<float> values_ ){
	values.assign( values_.begin(), values_.end() );
	is_full = true;
	oldest_value = 0;
}

void mm::add( float value ){
	values[oldest_value] = value;
	oldest_value++;
	if( oldest_value >= values.size() ){
		oldest_value = 0;
		is_full = true;
	}
}


int mm::size(){
	return values.size();
}
