#include <iostream>
#include <cstdio>
#include <vector>
#include "approach_2.hpp"
#include "mm.hpp"
#include "mytools.hpp"
#include "simulator.hpp"

#define SWAPE(x,y) {int t;t=x;x=y;y=t;}

using namespace std;

void load_source( vector<float> *source, string filename );
void load_adjusted_source( vector<float> *source, string trfn, string tefn, int sz );
void adjust_decision( vector<int> *decision );
void adjust_decision( vector<int> *decision, int sz );
vector<int> decision_ma( vector<float> vsource, int period1, int period2 );

int main( int argc, char** argv){
	
	if( argc != 5 ){
		cerr << "Usage:\n%s <csv training> <csv test> <short moving average> <long moving average>" << endl;
		return( 1 );
	}
	
	string filenameTR = argv[1];
	string filenameTE = argv[2];
	int p1 = atoi(argv[3]);
	int p2 = atoi(argv[4]);
	if( p1 > p2 ) SWAPE(p1,p2);
	
	mm sma, lma; //MM curta e longa
	sma.set_size( p1 );
	lma.set_size( p2 );
	
	vector<float> source;
	load_adjusted_source( &source, filenameTR, filenameTE, p2 );
	
	vector<int> decision = decision_ma( source, p1, p2 );
	//ajusta o vetor de decisao
	adjust_decision( &decision, p2 );
	
	simulator sim;
	sim.set_taxes( 0.0025 ); //custo de uma operacao de compra ou venda - 0,25% do volume
	float result = sim.run( source, decision ); //executa simulacao
	//~ *post = sim.pTrades();
	//~ *negt = sim.nTrades();

	//~ return *result;
}

//Carrega os arquivos de treino e teste, adicionando ao inicio de teste um 
// numero especifico de dados de treino a fim de parear dados da decisao
void load_adjusted_source( vector<float> *source, string trfn, string tefn, int sz ){
	vector<float> vaux;
	load_source( &vaux, trfn );
	source->assign(vaux.end()-sz+1, vaux.end());
	
	load_source( &vaux, tefn );
	source->insert( source->end(), vaux.begin(), vaux.end() );
}


void load_source( vector<float> *source, string filename ){
	ifstream infile;
	infile.open( filename );
	string line;

	source->clear();
	while( !infile.eof() ){
		getline( infile, line );
		infile >> ws;
		float price;
        //funcao de mytools.hpp
		get_prices_yahoo( &price, line );
		source->push_back(price);
	}
	infile.close();
}

void adjust_decision( vector<int> *decision, int sz ){
	decision->assign( decision->begin()+sz-1, decision->end() );
	adjust_decision( decision );
}

void adjust_decision( vector<int> *decision ){
	//elimina decisoes redundantes (compra seguido de compra, etc)
	int last_decision = 0;
	for( int i = 0; i < decision->size(); i++){
		if( (*decision)[i] != 0 )
			if( (*decision)[i] == last_decision )
				(*decision)[i] = 0;
			else
				last_decision = (*decision)[i];
	}

	//fecha ultima operacao em aberto, se houver
	//~ int sum = 0;
	//~ for( int i = 0; i < decision->size(); i++){
		//~ sum += (*decision)[i];
	//~ }
	//~ if( sum != 0 ){
		//~ (*decision)[ decision->size()-1 ] = sum * -1;
	//~ }
	(*decision)[ decision->size()-1 ] = last_decision * -1;

	//escreve em arquivo
	ofstream f_decision;
	f_decision.open( "00decision" );
	for( int i = 0; i < decision->size(); i++){
		f_decision << (*decision)[i] << endl;
	}
	f_decision.close();
}



//cria o vetor de decisao de uma estrategia de cruzamento de medias moveis
vector<int> decision_ma( vector<float> vsource, int period1, int period2 ){
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
