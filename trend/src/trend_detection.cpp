#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include "nikolov1.hpp"
#include "nikolov2.hpp"
#include "signal.hpp"
#include "signalSource.hpp"
#include "mytools.hpp"
#include "params.hpp"
#include "set_util.hpp"
#include "approach_1.hpp"
#include "approach_2.hpp"
#include "approach_3.hpp"
#include "simulator.hpp"


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
//~ float run( int *post, int *negt, float *result );
int run( simulator* sim );
string simulator_result_to_string( simulator sim );
string header_csv_to_string();
//void get_prices_test( float *price, string line );
string save_best_parameters( float result );
bool load_files_config( string configfn, vector<int>* smap, vector<int> *lmap, vector<string> *trsourcefn, vector<string> *tesourcefn );


//~ globais
params parameters;

//~
//~ Funcao principal
//~
int main( int argc, char *argv[] ){

	//configurar arquivo de log
	std::filebuf flog ;
    flog.open( LOG_FN, std::ios::out ) ;
    std::clog.rdbuf( &flog ) ;

	//inicializar parametros
	//~ params parameters;
	if( argc > 2 ){
		if( !parameters.initialize( argc, argv ) ) {
			cout << "Initialization error. Bad, bad parameters..." << endl;
			return 1;
		}
		//~ int post, negt;
		//~ float result;
		//~ run( &post, &negt, &result );
		simulator simResult;
		run( &simResult );
		cout << simulator_result_to_string( simResult ) << endl;
		//~ cout << parameters.to_string_csv() << post << " " << negt << " " << result << endl;

	} else {

		// linhas de teste
		float gamaVec[] = {0.1, 1, 10};
		float thetaVec[] = {0.65, 1, 3};
		int detLimVec[] = {1, 3, 5};
		int nSmoothVec[] = {1, 5, 10, 20, 40};
		int nRefVec[] = {10, 30, 50, 80, 100};
		int nObsVec[] = {5, 10, 20, 40, 80};
		vector<int> smaP = {5};		//short moving average period
		vector<int> lmaP = {15};	//long moving average period
		vector<string> trSourceFN = {"00seno.out"};
		vector<string> teSourceFN = {"00cosseno.out"};

		string configfn = CONFIG_FN;
		if( argc == 2 ){
			configfn = argv[1];
		}

		//le arquivo de configuracao, salvando opcoes nos vetores
		if( !load_files_config( configfn, &smaP, &lmaP, &trSourceFN, &teSourceFN ) ){
			cout << "Initialization error. Bad, bad config_file..." << endl;
			return 1;
		}

		for( int i = 0; i < trSourceFN.size(); i++ ){
			cout << "TR: " << trSourceFN[i] << "; TE: " << teSourceFN[i] << endl;

			//abre arquivo de resultado
			ofstream f_result;
			f_result.open( teSourceFN[i] + "TestResult" );
			//~ f_result << parameters.header_csv() << parameters.sep << "PosTrades" << parameters.sep << "NegTrades" << parameters.sep << "Result" << endl;
			f_result << header_csv_to_string() << endl;

			for( float g : gamaVec ){
				for( float t : thetaVec ){
					for( int dl : detLimVec ){
						for( int nsm : nSmoothVec ){
							for( int nre : nRefVec ){
								for( int nob : nObsVec ){
									if( nob > nre ) continue; //condicao invalida onde o sinal observado eh maior que o referencia

									cout << "." << flush;

									string tefn = teSourceFN[i] + std::to_string(nob);

									parameters.set_params( g, t, dl, nsm, nre, nob, smaP[i], lmaP[i], trSourceFN[i], tefn );
									simulator simResult;
									run( &simResult );
									
									f_result << simulator_result_to_string( simResult ) << endl;
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

int run( simulator *sim ){
	//conjuntos de exemplos positivos e negativos
	vector<signal> rplus, rminus, rzero;
	set_util setutil; //operacoes nos conjuntos
	approach_2 approach; //segunda abordagem (com medias moveis e 1 nikolov)
	//~ approach_3 approach; //terceira abordagem (com medias moveis e 2 nikolovs)
	nikolov1 niko; //1 nikolov apenas
	//~ nikolov2 niko; //2 nikolovs

	//serie temporal
	signalSource sigSource;
	load_source( &sigSource, parameters.trFN.c_str() );
	approach.build_sets( sigSource, &rplus, &rminus, &rzero );

	//carrega os conjuntos positivo, negativo e zero
	//~ if( rplus.empty() || rminus.empty() || rzero.empty() ){
		//~ cout << "At least one set is empty. Aborting." << endl;
		//~ return 1;
	//~ }
	//~ cout << rplus.size() << " " << rminus.size() << " ";
	setutil.transform( &rplus );
	setutil.transform( &rminus );
	setutil.transform( &rzero );

	//~ load_source( &sigSource, TEST_SOURCE_FN );
	load_source( &sigSource, parameters.teFN.c_str() );

	vector<int> decision;
	decision = niko.detect( sigSource, rplus, rminus, rzero, params::gama, params::theta, params::detectionsLimit );

	//ajusta o vetor de decisao
	adjust_decision( &decision );

	//~ simulator sim;
	sim->set_taxes( 0.0025 ); //custo de uma operacao de compra ou venda - 0,25% do volume
	sim->run( sigSource.getSource(), decision ); //executa simulacao
	//~ *post = sim.pTrades();
	//~ *negt = sim.nTrades();

	return 0;
}

string simulator_result_to_string( simulator sim ){
	ostringstream result;
	result << parameters.to_string_csv();
	result << params::sep;
	result << sim.positive_trades();
	result << params::sep;
	result << sim.negative_trades();
	result << params::sep;
	result << sim.trades_accuracy();
	result << params::sep;
	result << sim.average_win();
	result << params::sep;
	result << sim.average_loss();
	result << params::sep;
	result << sim.final_returns();
	return result.str();
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


bool load_files_config( string configfn, vector<int>* smap, vector<int> *lmap, vector<string> *trsourcefn, vector<string> *tesourcefn ){

	smap->clear();
	lmap->clear();
	trsourcefn->clear();
	tesourcefn->clear();

	ifstream f_config (configfn);
	if( !f_config.is_open() )
		return false;

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

	return true;
}


void load_source( signalSource *sigSource, const char* filename ){
	sigSource->clear();
	ifstream infile;
	infile.open( filename );
	string line;

	while( !infile.eof() ){
		getline( infile, line );
		float price;
//		get_prices_test( &price, line );
        //funcao de mytools.hpp
		get_prices_yahoo( &price, line );
		sigSource->add( 0, price );
	}
	sigSource->finish();
	infile.close();
}

//void get_prices_test( float *price, string line ){
//	sscanf(line.c_str(), "%f", price);
//}

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

string header_csv_to_string(){
	ostringstream result;
	result << parameters.header_csv();
	result << params::sep;
	result << "PosTrades";
	result << params::sep;
	result << "NegTrades";
	result << params::sep;
	result << "Acc";
	result << params::sep;
	result << "AvgWin";
	result << params::sep;
	result << "AvgLoss";
	result << params::sep;
	result << "Return";
	return result.str();
}
 


//~ Cabecalho da saida padrao, , ,
//~ gama,theta,detectionsLimit,NSMOOTH,NREF,NOBS,sma,lma,R+size,R-Size,+Trades,-Trades,Acc,Return
//~ gama,theta,detectionsLimit,NSMOOTH,NREF,NOBS,sma,lma,+Trades,-Trades,Acc,AvgWin,AvgLoss,Return


