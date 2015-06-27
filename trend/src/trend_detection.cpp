#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include "../src/nikolov.hpp"
#include "../src/signal.hpp"
#include "../src/signalSource.hpp"
#include "../src/mytools.hpp"
#include "../src/params.hpp"
#include "../src/detection.hpp"
#include "../src/set_util.hpp"
#include "../src/approach_1.hpp"
#include "../src/approach_2.hpp"
#include "../src/simulator.hpp"


//~ #define TRAINING_SOURCE_FN "00seno.out"
//~ #define TEST_SOURCE_FN "00cosseno.out"
//~ #define RPLUS_FN "../data/rplus"
//~ #define RMINUS_FN "../data/rminus"
//~ #define TRENDS_FN "../data/trend"
#define LOG_FN "log"
#define DETECTIONS_FN "detections"
#define CONFIG_FN "00test_config"


//~ funcoes
void load_source( signalSource *sigSource, const char* filename );
void adjust_decision( vector<int> *decision );
float run( int *post, int *negt, float *result );
void get_prices_test( float *price, string line );
string save_best_parameters( float result );
void load_files_config( vector<int>* smap, vector<int> *lmap, vector<string> *trsourcefn, vector<string> *tesourcefn );


//~ 
//~ Funcao principal
//~ 
int main( int argc, char *argv[] ){

	//configurar arquivo de log
	std::filebuf flog ;
    flog.open( LOG_FN, std::ios::out ) ;
    std::clog.rdbuf( &flog ) ;
	
	//inicializar parametros
	params parameters;
	if( argc > 1 ){
		if( !parameters.initialize( argc, argv ) ) {
			cout << "Initialization error. Bad, bad parameters..." << endl;
			return 1;
		}
		int post, negt;
		float result;
		run( &post, &negt, &result );
		cout << parameters.to_string_csv() << post << " " << negt << " " << result << endl;
	
	} else {
		
		// linhas de teste
		float gamaVec[] = {0.1, 1, 10};
		float thetaVec[] = {0.65, 1, 3};
		int detLimVec[] = {1, 3, 5};
		int nSmoothVec[] = {1, 5, 10};
		int nRefVec[] = {10, 30, 50};
		int nObsVec[] = {5, 10, 20, 40};
		vector<int> smaP = {5};		//short moving average period
		vector<int> lmaP = {15};	//long moving average period
		vector<string> trSourceFN = {"00seno.out"};
		vector<string> teSourceFN = {"00cosseno.out"};
		

		//le arquivo de configuracao, salvando opcoes nos vetores 
		load_files_config( &smaP, &lmaP, &trSourceFN, &teSourceFN );
		
		for( int i = 0; i < trSourceFN.size(); i++ ){
			cout << "TR: " << trSourceFN[i] << "; TE: " << teSourceFN[i] << endl;
			
			//abre arquivo de resultado
			ofstream f_result;
			f_result.open( teSourceFN[i] + "TestResult" );
			f_result << parameters.header_csv() << parameters.sep << "PosTrades" << parameters.sep << "NegTrades" << parameters.sep << "Result" << endl;
			
			for( float g : gamaVec ){
				for( float t : thetaVec ){
					for( int dl : detLimVec ){
						for( int nsm : nSmoothVec ){
							for( int nre : nRefVec ){
								for( int nob : nObsVec ){
									if( nob > nre ) continue; //condicao invalida onde o sinal observado eh maior que o referencia
									
									cout << "." << flush;
									
									parameters.set_params( g, t, dl, nsm, nre, nob, smaP[i], lmaP[i], trSourceFN[i], teSourceFN[i] );
									int post, negt;
									float result;
									run( &post, &negt, &result );

									//escreve resultado em arquivo
									f_result << parameters.to_string_csv() << params::sep << post << params::sep << negt << params::sep << result << endl;
								}
							}
						}
					}
				}
			}
			//fecha arquivo de rsultado
			f_result.close();
			cout << endl;
		}
	}
}

float run( int *post, int *negt, float *result ){
	//conjuntos de exemplos positivos e negativos
	vector<signal> rplus, rminus;
	set_util setutil; //operacoes nos conjuntos
	approach_2 approach; //segunda abordagem (com medias moveis)

	params parameters;
	//~ cout << parameters.to_string_csv() << " ";

	
	//serie temporal
	signalSource sigSource;
	//~ load_source( &sigSource, TRAINING_SOURCE_FN );
	load_source( &sigSource, parameters.trFN.c_str() );
	approach.build_sets( sigSource, &rplus, &rminus );
	
	//carrega os conjuntos positivo e negativo
	if( rplus.empty() || rminus.empty() ){
		cout << "At least one set is empty. Aborting." << endl;
		return 1;
	}
	//~ cout << rplus.size() << " " << rminus.size() << " ";
	setutil.transform( &rplus );
	setutil.transform( &rminus );
	
	//~ load_source( &sigSource, TEST_SOURCE_FN );
	load_source( &sigSource, parameters.teFN.c_str() );
	
	vector<int> decision;
	decision = detect( sigSource, rplus, rminus, params::gama, params::theta, params::detectionsLimit );

	//ajusta o vetor de decisao
	adjust_decision( &decision );
	
	simulator sim;
	sim.set_taxes( 0.0025 ); //custo de uma operacao de compra ou venda - 0,25% do volume
	(*result) = sim.run( sigSource.getSource(), decision ); //executa simulacao	
	*post = sim.pTrades();
	*negt = sim.nTrades();
	
	return *result;
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
	int sum = 0;
	for( int i = 0; i < decision->size(); i++){
		sum += (*decision)[i];
	}
	if( sum != 0 ){
		(*decision)[ decision->size()-1 ] = sum * -1;
	}
	
	//escreve em arquivo
	ofstream f_decision;
	f_decision.open( "00decision" );
	for( int i = 0; i < decision->size(); i++){
		f_decision << (*decision)[i] << endl;
	}
	f_decision.close();
}


void load_files_config( vector<int>* smap, vector<int> *lmap, vector<string> *trsourcefn, vector<string> *tesourcefn ){
	
	smap->clear();
	lmap->clear();
	trsourcefn->clear();
	tesourcefn->clear();
	
	ifstream f_config (CONFIG_FN);
	string line;
	
	getline( f_config, line );	//descarta cabecalho
	while( !f_config.eof() ){
		//~ getline( infile, line );
		int p;
		f_config >> p;
		smap->push_back(p);
		f_config >> p;
		lmap->push_back(p);
		
		string fn; 
		f_config >> fn;
		trsourcefn->push_back(fn);
		f_config >> fn >> ws;
		tesourcefn->push_back(fn);
		
	}
	f_config.close();
}


void load_source( signalSource *sigSource, const char* filename ){
	sigSource->clear();
	ifstream infile;
	infile.open( filename );
	string line;
	
	while( !infile.eof() ){
		getline( infile, line );
		float price;
		get_prices_test( &price, line );
		sigSource->add( 0, price );
	}
	sigSource->finish();
	infile.close();
}

void get_prices_test( float *price, string line ){
	sscanf(line.c_str(), "%f", price);
}

string save_best_parameters( float result ){
	params parameters;
	string str;
	str += parameters.to_string_csv();
	str += result;
	return str;
}


/*string final_result_header(){
	string line;
	//~ line = "Detected,";
	line += "+Trades,";
	line += "-Trades,";
	line += "Acc,";
	//~ line += "InitS,";
	//~ line += "FinalS,";
	line += "Return";
	return line;
}*/



//~ Cabecalho da saida padrao, , , 
//~ gama,theta,detectionsLimit,NSMOOTH,NREF,NOBS,sma,lma,R+size,R-Size,+Trades,-Trades,Acc,Return
//~ gama,theta,detectionsLimit,NSMOOTH,NREF,NOBS,sma,lma,R+size,R-Size,+Trades,-Trades,Acc,Return


