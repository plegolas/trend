#include "approach_2.hpp"

//

//criar um conjunto de sinais positivos e negativos baseando-se em 
// cruzamentos de medias moveis
void approach_2::build_sets( signalSource source, vector<signal> *rplus, vector<signal> *rminus ){
	//testa tamanho da fonte com tamanho da maior MM
	if( source.lenght() < params::lma_period ){
		clog << "ERROR: source shorter than long MA" << endl;
		exit( EXIT_FAILURE );
	}
	
	//obtem o vetor de decisao
	vector<float> vsource = source.getSource(); //vetor de valores
	vector<int> decision = decision_ma( vsource, params::sma_period, params::lma_period );
	
	//debug
	//~ for( int i =0; i <decision.size();i++){
		//~ cout << decision[i] << endl;
	//~ }
	
	//cria os conjuntos R+ e R- baseando-se no vetor de decisao das MMs
	int sigSize = params::NREF + params::NSMOOTH; //tamanho minimo da referencia
	for( int i = sigSize; i < decision.size(); i++ ){
		if( decision[i] == 0 ) continue;
		if( decision[i] == 1 ){ //cross up
			vector<float> vaux( &(vsource[i-sigSize+1]), &(vsource[i+1]) );
			signal s( vaux );
			rplus->push_back( s );
		} else if( decision[i] == -1 ){ //cross down
			vector<float> vaux( &(vsource[i-sigSize+1]), &(vsource[i+1]) );
			signal s( vaux );
			rminus->push_back( s );
		}
	}
	
	//debug
	//~ cout << "rminus" << endl;
	//~ for( signal s : *rminus ){
		//~ s.print();
	//~ } 	
	//~ cout << "rplus" << endl;
	//~ for( signal s : *rplus ){
		//~ s.print();
	//~ } 	
}


//cria o vetor de decisao de uma estrategia de cruzamento de medias moveis
vector<int> approach_2::decision_ma( vector<float> vsource, int period1, int period2 ){
	int i = 0;
	mm sma, lma; //MM curta e longa
	sma.set_size( (period1<period2)?period1:period2 );
	lma.set_size( (period1>period2)?period1:period2 );
	
	vector<float> sma_avg( vsource.size(), 0 ); //media da mms
	vector<float> lma_avg( vsource.size(), 0 ); //media da mml
	vector<int> decision( vsource.size(), 0 ); //vetor de decisao
	
	//preenche um vetor com todos os valores das MMs
	for( i = 0; i < vsource.size(); i++ ){
		float f = vsource[i];
		sma.add( f );
		sma_avg[i] = sma.average();
		
		lma.add( f );
		lma_avg[i] = lma.average();
		
		//debug	
		//~ cout << sma_avg[i] << ", " << lma_avg[i] << endl;
	}

	//verifica os cruzamentos montando o vetor de decisao
	int begin = lma.size()-1;
	float difAnt = sma_avg[begin] - lma_avg[begin];
	for( i = begin; i < vsource.size(); i++ ){
		//verifica cruzamento
		float dif = sma_avg[i] - lma_avg[i];
		if( dif > 0 && difAnt < 0 ){
			decision[i] = 1;
		} else if ( dif < 0 && difAnt > 0 ){
			decision[i] = -1;
		} else if ( dif == 0 && difAnt < 0 ){
			decision[i] = 1;
			dif = 1;
		} else if ( dif == 0 && difAnt > 0 ){
			decision[i] = -1;
			dif = -1;
		}
		difAnt = dif;

/*		//antigo teste de verificacao de cruzamento
		if( sma_avg[i-1] < lma_avg[i-1] && sma_avg[i] > lma_avg[i] ){ //cross up
		} else if ( last_sma > last_lma && sma_avg < lma_avg ){ //cross down
		} */
	}	
	
	return decision;
}
