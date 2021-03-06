#include "nikolov2.hpp"

//-----------------------------

vector<int> nikolov2::detect( signalSource source, vector<signal> rplus, vector<signal> rminus, vector<signal> rzero,
				double gama, double theta, int detectionsLimit ){
	
	source.configure( params::NOBS + params::NSMOOTH );
	vector<int> detections ( source.lenght(), 0 ); //vetor de decisao
	int dIdx = params::NOBS + params::NSMOOTH -1; //indice do vetor de decisao
	//TODO: verificar sincronia do dIdx com source
	
	_position position = pout;
	int cpd = 0; //consecutive positive detections
	int cnd = 0; //consecutive negative detections
	dIdx--;
	do{
		dIdx++;
		signal s = source.nextBlock(); //updateObservations function
		s.transform();
		s.pop_front( params::NSMOOTH );
		
		double prob;
		//test detection for long positions (buy)
		if( position != plong ){
			prob = nikosProbability( s, rplus, rzero, gama);
			if( detection( prob, theta, &cpd, detectionsLimit ) ){
				detections[dIdx] = 1;
				position = plong;
				cnd = 0;
				continue;
			}
		}
		
		//test detection for short positions (sell)
		if( position != pshort ){
			prob = nikosProbability( s, rminus, rzero, gama);
			if( detection( prob, theta, &cnd, detectionsLimit ) ){
				detections[dIdx] = -1;
				position = pshort;
				cpd = 0;
			}
		}
	}while(source.hasData());
	
	return detections;
}


//----------------------------

int nikolov2::detection( double prob, double theta, int *counter, int detectionsLimit ){
	if( prob > theta ){
		(*counter)++;
		if( *counter >= detectionsLimit ){
			*counter = 0;
			return 1;
		}
	} else {
		*counter = 0;
	}
	return 0;
}

//----------------------------

double nikolov2::nikosProbability( signal s, vector<signal> rplus, vector<signal> rminus, double gama){
	//positive distance
	list<float> posDist;
	posDist.clear();
	for( signal r : rplus ){
		float dist = distToReference( s, r );
		posDist.push_back( dist );
	}

	//negative distance
	list<float> negDist;
	negDist.clear();
	for( signal r : rminus ){
		float dist = distToReference( s, r );
		negDist.push_back( dist );
	}

	double probPos = probClass(posDist, gama);
	double probNeg = probClass(negDist, gama);
	double R = probPos / probNeg;
	return R;
}

//----------------------------

float nikolov2::distToReference( signal s, signal r ){
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

float nikolov2::dist( signal s, signal t ){
	float d = 0;
	for( int i = 0; i < s.lenght(); i++ ){
		d += pow( s.at(i)-t.at(i), 2 );
	}
	return d;
}


//----------------------------

double nikolov2::probClass( list<float> dist, double gama ){
	double p = 0;
	for( float d : dist ){
		p += exp( -1*gama*d );
	}
	return p;
}

//----------------------------

