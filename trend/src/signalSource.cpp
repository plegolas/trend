#include "signalSource.hpp"

void signalSource::clear(){
	serie.clear();
	itFirstValue = itLastValue = serie.end();
}

bool signalSource::hasData(){
	return (itLastValue != serie.end());
}

pair<time_t,float>  signalSource::lastValue(){
	SOURCEIT it = itLastValue;
	return *(--it);
}

pair<time_t,float>  signalSource::nextValue(){
	pair<time_t,float> ret;
	if( hasData() ){
		ret = *itLastValue;
		itLastValue++;
		itFirstValue++;
	}
	return ret;
}

void signalSource::print(){
	SOURCEIT it = serie.begin();
	while( it != serie.end() ){
		cout << time_t2string( it->first ) << "," << it->second << endl; 
		it++;
	}
}

//comparition used in the configure function
//~ bool signalSource::my_compare (const std::pair<time_t,float>& first, 
						//~ const std::pair<time_t,float>& second){
	//~ return ( first.first < second.first );
//~ }

void signalSource::add( time_t date, float value ){
	serie.push_back( make_pair(date, value) );
}

//~ void signalSource::add( time_t date, float value ){
	//~ serie.push_front( make_pair(date, value) );
//~ }

//~ void signalSource::finish(){
	//~ serie.pop_front(); //bug fix
//~ }

void signalSource::finish(){
	serie.pop_back(); //bug fix
}

void signalSource::configure( int nPos ){
	itFirstValue = itLastValue = serie.begin();
	for( int i = 0; i < nPos; i++ )
		itLastValue++;
}

float signalSource::at( int pos ){ 
	SOURCEIT it = serie.begin();
	for( int i = 0; i<pos; i++, it++ );
	return it->second; 
}

time_t signalSource::get_time(){
	SOURCEIT it = itLastValue;
	//~ it--;
	return (--it)->first;
}

signal signalSource::lastBlock(){
	signal s;
	SOURCEIT it = itFirstValue;
	while( it != itLastValue ){
		s.add( it->second );
		it++;
	}
	return s;
}

signal signalSource::nextBlock(){
	return nextBlock(0);
}

signal signalSource::nextBlock( int skip_size ){
	signal s;
	if( !hasData() )
		return s;
	
	//jump to the next and skip some data
	while( skip_size-- >= 0 && hasData() ){
		itFirstValue++;
		itLastValue++;
	}
	
	return lastBlock();
}
