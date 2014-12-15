#include "nikolov.hpp"

//-----------------------------

list< pair<time_t,signal> > detect( signalSource source, vector<signal> rplus, vector<signal> rminus, 
				double gama, double theta, int detectionsLimit ){
	
	source.configure( params::NOBS + params::NSMOOTH );
	list< pair<time_t,signal> > detections;
	
	int cd = 0; //consecutive detections
	do{
		signal s = source.nextBlock(); //updateObservations function
		s.transform();
		s.pop_front( params::NSMOOTH );
		
		//positive distance
		list<float> posDist;
		posDist.clear();
		for( signal r : rplus ){
			float dist = distToReference( s, r );
			posDist.push_back( dist );
			//~ posDist.push_back( distToReference( s, r ) );
		}
		
		//negative distance
		list<float> negDist;
		negDist.clear();
		for( signal r : rminus ){
			float dist = distToReference( s, r );
			negDist.push_back( dist );
			//~ negDist.push_back( distToReference( s, r ) );
		}
		
		double probPos = probClass(posDist, gama);
		double probNeg = probClass(negDist, gama);
		double R = probPos / probNeg;
		//~ double R = probClass(posDist, gama) / probClass(negDist, gama);
		
		if( R > theta ){
			cd++;
			if( cd >= detectionsLimit ){
				pair<time_t,signal> p (source.get_time(), s);
				detections.push_back( p );
				cd = 0;
			}
		} else {
			cd = 0;
		}
	}while(source.hasData());
	
	return detections;
}

//----------------------------

float distToReference( signal s, signal r ){
	int n = s.lenght();
	int m = r.lenght();
	
	float minDist = FLT_MAX;
	
	for( int i = 0; i < m-n+1; i++ ){
		float newDist = dist( s, r.subSignal(i, n) ); //dist( r.subSignal(i, i+n-1), s );
		minDist = (minDist<newDist)?minDist:newDist;// min( minDist, newDist );
	}
	return minDist;
}

//----------------------------

float dist( signal s, signal t ){
	float d = 0;
	for( int i = 0; i < s.lenght(); i++ ){
		d += pow( s.at(i)-t.at(i), 2 );
	}
	return d;
}


//----------------------------

double probClass( list<float> dist, double gama ){
	double p = 0;
	for( float d : dist ){
		p += exp( -1*gama*d );
	}
	return p;
}

//----------------------------

