#include "approach_1.hpp"

//

//criar um conjunto de sinais positivos e negativos
void approach_1::build_sets( signalSource source, vector<signal> *rplus, vector<signal> *rminus ){
	source.configure( params::NREF + params::NSMOOTH );
	srand(time(NULL));
	//~ srand(1);
	
	int jump = 0; //quantidade de pontos a serem ignorados (skip)
	while( source.hasData() ){
		signal sig = source.nextBlock( jump );
		if( sig.trend_return( params::NTREND ) >= (float) params::NTRESH ){
			//~ sig.transform();
			rplus->push_back( sig );
			clog << "\tSet +: " << time_t2string( source.get_time() ) << endl;
			jump = 5;
		} else {
			jump = 0;
			//inserir somente 10% dos sinais, ate o limite rplus.size
			int random = rand() % 100;
			if( rminus->size() < rplus->size() && random < 10 ){
				//~ sig.transform();
				rminus->push_back( sig );
				clog << "\tSet -: " << time_t2string( source.get_time() ) << endl;
			}
		}
	}
	clog << "R+ size: " << rplus->size() << endl;
	clog << "R- size: " << rminus->size() << endl;
	
	clog << "Sets R+ and R- created" << endl;
}
