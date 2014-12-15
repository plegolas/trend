#include <iostream>
#include <fstream>
#include <cstdlib>
#include "nikolov.hpp"
#include "signal.hpp"
#include "signalSource.hpp"
#include "mytools.hpp"
#include "params.hpp"
#include "detection.hpp"
#include "set_util.hpp"
#include "approach_1.hpp"

#define FLAG_CREATE_SETS "-c"

//~ #define TRAINING_SOURCE_FN "../data/training_data.csv"
//~ #define TEST_SOURCE_FN "../data/test_data.csv"
#define TRAINING_SOURCE_FN "../data/PETR4.SA1"
#define TEST_SOURCE_FN "../data/PETR4.SA3"
#define RPLUS_FN "../data/rplus"
#define RMINUS_FN "../data/rminus"
#define TRENDS_FN "../data/trend"
#define LOG_FN "log"
#define DETECTIONS_FN "detections"

void load_source( signalSource *sigSource, const char* filename ){
	sigSource->clear();
	ifstream infile;
	infile.open( filename );
	string line;
	
	while( 	!infile.eof() ){
		getline( infile, line );
		time_t date;
		float price;
		get_prices_yahoo( &date, &price, line );
		sigSource->add( date, price );
	}
	sigSource->finish();
	infile.close();
	clog << "Source data loaded" << endl;
}

string new_filename( string fn, int nb ){
	char buffer[10];
	sprintf(buffer, "%d", nb);
	fn.append( buffer );
	return fn;
}

detection get_detection( signalSource source, signal sig ){
	detection d;
	d.max = d.first = source.lastValue().second;
	d.time = source.lastValue().first;
	d.sig = sig;
	for( int i = 1; i < params::NTREND && source.hasData(); i++ ){
		float val = source.nextValue().second;
		if( val < 0.001 ) break; //------------- bug fix

		d.last = val;
		if( val > d.max ) 
			d.max = val;
	}
	//~ d.last = source.lastValue().second;
	d.after_detection = source.lastBlock();
	return d;
}

list<detection> trends_result( signalSource source, list< pair<time_t,signal> > trends ){
	list<detection> detections;
	source.configure( params::NTREND );
	for( pair<time_t,signal> p : trends ){
		//encontra inicio da tendencia
		while( source.hasData() && source.get_time() != p.first )
			source.nextValue();
		//preenche objeto detection
		detection d = get_detection( source, p.second );
		detections.push_back( d );
	}
	return detections;
}

void print_detections( string fn, list<detection> *detections ){
	//date first last max
	ofstream out (fn);
	for( detection d : *detections ){
		out << time_t2string(d.time) << ","
			<< d.first << ","
			<< d.last << ","
			<< d.max << endl; 
	}
	out.close();
}
	
void final_result( list<detection> *detections ){
	int pos, neg;
	pos = neg = 0;
	float init_funds, funds;
	init_funds = funds = 10000;
	float profit = 0;
	
	for( detection d : *detections ){
		if( d.first > d.last )
			neg++;
		else
			pos++;
		
		float p = d.last/d.first;
		funds *= p;
	}
	cout << "Trends detected: " << detections->size() << endl;
	cout << "Positive trades: " << pos << endl;
	cout << "Negative trades: " << neg << endl;
	cout << "Accuracy: " << (float)pos/((float)pos+(float)neg)*100 << "%" << endl;
	cout << "Initial funds: " << init_funds << endl;
	cout << "Final funds: " << funds << endl;
	cout << "Return: " << ((funds/init_funds)-1)*100 << "%" << endl;
}

string final_result_csv( list<detection> *detections, int plussz, int minussz ){
	int pos, neg;
	pos = neg = 0;
	float init_funds, funds;
	init_funds = funds = 10000;
	float profit = 0;
	
	for( detection d : *detections ){
		if( d.first > d.last )
			neg++;
		else
			pos++;
		
		float p = d.last/d.first;
		funds *= p;
		if( funds < 1 ){
			funds = funds;
		}
	}
	
	float accu = (float)pos/((float)pos+(float)neg)*100;
	float fund_ret = ((funds/init_funds)-1)*100;
	//~ "Trends detected" "Positive trades" "Negative trades" "Accuracy" 
	//~ "Initial funds" "Final funds" "Return"
	char cline[300];
	//~ sprintf( cline, "%d,%d,%d,%.2lf,%.2f,%.2f,%.2f", (int)detections->size(),
		//~ pos, neg, accu, init_funds, funds, fund_ret );
	sprintf( cline, "%5d %5d %5d %5d %5d\t%.2lf\t%8.2f\t%8.2f\t%+6.2f%%", plussz, minussz, (int)detections->size(),
		pos, neg, accu, init_funds, funds, fund_ret );
	
	std::string line = cline;
	return line;
}

string final_result_header(){
	string line;
	line = "Detected,";
	line += "+Trades,";
	line += "-Trades,";
	line += "Acc,";
	line += "InitS,";
	line += "FinalS,";
	line += "Return";
	return line;
}

//verifica se devem ser criados os conjuntos positivo e negativo
bool is_sets_creation( int argc, char *argv[] ){
	if( argc > 1 ){
		for( int i = 1; i < argc; i++ ){
			if( strcmp( argv[i], FLAG_CREATE_SETS ) == 0 ){
				return true;
			}
		}
	}
	return false;
}

int main( int argc, char *argv[] ){

	//configurar arquivo de log
	std::filebuf flog ;
    flog.open( LOG_FN, std::ios::out ) ;
    std::clog.rdbuf( &flog ) ;
	
	//inicializar parametros
	params parameters;
	if( !parameters.initialize( argc, argv ) ) {
		cout << "Initialization error. Bad, bad parameters..." << endl;
		return 1;
	}
	//~ parameters.print();
	
	//conjuntos de exemplos positivos e negativos
	vector<signal> rplus, rminus;
	set_util setutil; //operacoes nos conjuntos
	approach_1 approach; //primeira abordagem
	
	//serie temporal
	signalSource sigSource;
	
	//~ if( is_sets_creation(argc, argv) ){
		//carrega a serie temporal, constroi os conjuntos positivo e 
		// negativo e os salva em arquivos
		clog << "Build sets mode" << endl;
		load_source( &sigSource, TRAINING_SOURCE_FN );
		approach.build_sets( sigSource, &rplus, &rminus );
		//~ setutil.print( RPLUS_FN, &rplus);
		//~ setutil.print( RMINUS_FN, &rminus);
		//~ return 0;
	//~ }
	
	clog << "Run mode" << endl;
	//~ cout << parameters.header_csv() << "," << final_result_header() << endl;
	//~ cout << parameters.to_string_csv() << ",";
	cout << parameters.to_string_csv() << "\t";
	
	//carrega os conjuntos positivo e negativo
	//~ setutil.load( &rplus, RPLUS_FN );
	//~ setutil.load( &rminus, RMINUS_FN );
	if( rplus.empty() || rminus.empty() ){
		cout << "At least one set is empty. Aborting." << endl;
		//~ cout << "-------------" << endl;
		return 1;
	}
	setutil.transform( &rplus );
	setutil.transform( &rminus );
	
	load_source( &sigSource, TEST_SOURCE_FN );
	
	
	//~ cout << "Detecting... " << endl;
	
	list< pair<time_t,signal> > trends;
	trends = detect( sigSource, rplus, rminus, params::gama, params::theta, params::detectionsLimit );
	
	list<detection> detections = trends_result( sigSource, trends );
	
	print_detections( DETECTIONS_FN, &detections );
	//~ final_result( &detections );
	cout << final_result_csv( &detections, rplus.size(), rminus.size() ) << endl;
	
	//~ cout << "-------------" << endl;
}


//~ Cabecalho da saida padrao, , , 
//~ gama,theta,detectionsLimit,NSMOOTH,NREF,NOBS,NTREND,NTRESH,R+size,R-Size,Detected,+Trades,-Trades,Acc,InitS,FinalS,Return
//~ gama,theta,detectionsLimit,NSMOOTH,NREF,NOBS,NTREND,NTRESH,R+size,R-Size,Detected,+Trades,-Trades,Acc,InitS,FinalS,Return

