#include "approach_2.hpp"

//

//criar um conjunto de sinais positivos e negativos baseando-se em 
// cruzamentos de medias moveis
void approach_2::build_sets( signalSource source, vector<signal> *rplus, vector<signal> *rminus, vector<signal> *rzero ){
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
	
	//cria os conjuntos R+, R- e R0 baseando-se no vetor de decisao das MMs
	int sigSize = params::NREF + params::NSMOOTH; //tamanho minimo da referencia
	for( int i = sigSize; i < decision.size(); i++ ){
		if( decision[i] == 0 ) continue;

		vector<float> vaux( &(vsource[i-sigSize+1]), &(vsource[i+1]) );
		signal s( vaux );
		if( decision[i] == 1 ){ //cross up
			if( positive_trade( vsource, decision, i, 1 ) )
				rplus->push_back( s );
			else
				rzero->push_back( s );
		} else if( decision[i] == -1 ){ //cross down
			if( positive_trade( vsource, decision, i, -1 ) )
				rminus->push_back( s );
			else
				rzero->push_back( s );
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

int approach_2::positive_trade( vector<float> source, vector<int> decision, int i, int order_side ){
	float price_in = source[i];
	float price_out = 0;
	for( ; i < source.size(); i++ )
		if( decision[i] == order_side*-1 )
			break;
	if( i >= source.size() ) 
		i--;
	price_out = source[i];
	price_out -= price_in;
	price_out *= order_side;
	return (price_out > 0);
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
	}	
	
	return decision;
}
