#include "params.hpp"

char FLAG_CREATE_SETS[] = "-c";

double params::gama = 0.1;
double params::theta = 0.65;
int params::detectionsLimit = 3;
int params::NSMOOTH = 5;

int params::NREF = 50;
int params::NOBS = 40;
int params::NTREND = 15;
double params::NTRESH = 0.1;

int params::sma_period = 18;
int params::lma_period = 30;


void params::print(){
	std::cout << "gama: " << gama << std::endl;
	std::cout << "theta: " << theta << std::endl;
	std::cout << "detectionsLimit: " << detectionsLimit << std::endl;
	std::cout << "NSMOOTH: " << NSMOOTH << std::endl;
	std::cout << "NREF: " << NREF << std::endl;
	std::cout << "NOBS: " << NOBS << std::endl;
	std::cout << "NTREND: " << NTREND << std::endl;
	std::cout << "NTRESH: " << NTRESH << std::endl;
}

std::string params::header_csv(){
	std::string line;
	line = "gama,";
	line += "theta,";
	line += "detectionsLimit,";
	line += "NSMOOTH,";
	line += "NREF,";
	line += "NOBS,";
	line += "NTREND,";
	line += "NTRESH";
	return line;
}

std::string params::to_string_csv(){
	char cline[200];
	//~ sprintf( cline, "%.2lf,%.2lf,%d,%d,%d,%d,%d,%.2lf", gama, theta, detectionsLimit,
		//~ NSMOOTH, NREF, NOBS, NTREND, NTRESH );
	sprintf( cline, "%4.2lf %4.2lf %3d %3d %3d %3d %3d %4.2lf ", gama, theta, detectionsLimit,
		NSMOOTH, NREF, NOBS, NTREND, NTRESH );
	std::string line = cline;
	return line;
}
	
bool params::initialize( int argc, char *argv[] ){
	params_count = 8;
	
	//sem parametros, execucao com variaveis padrao
	if( argc == 1 )
		return true;
	
	//~ //a flag indicando modo de criacao de conjuntos
	//~ if( argc > 1 && strcmp(argv[argc-1],flag) == 0 )
		//~ return true;
	
	//quantidade errada de parametros
	if( argc < params_count +1 ){
		std::clog << "Init error. argc = " << argc << ", params_count = " << params_count << std::endl;
		return false;
	}
	
	//quantidade correta de parametros, atribui os novos valores
	gama = atof(argv[1]);
	theta = atof(argv[2]);
	detectionsLimit = atoi(argv[3]);
	NSMOOTH = atoi(argv[4]);
	NREF = atoi(argv[5]);
	NOBS = atoi(argv[6]);
	NTREND = atoi(argv[7]);
	NTRESH = atof(argv[8]);
	
	return true;
}


/*
 * Foram exploradas todas as combinacoes possiveis para as listagens
 * dos parametros a seguir, exceto as incompativeis (Nobs > Nref, p. ex.).
 * Para cada combinacao foram executados 5 testes aleatorios.
 * 
 * gama = 0.1, 1, 10
 * Nobs = 10, 80, 115, 150
 * Nsmooth = 10, 80, 115, 150
 * Href = 3, 5, 7, 9
 * Dreq = 1, 3, 5
 * theta = 0.65, 1, 3
 * 
 * */
