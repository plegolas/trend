#ifndef PARAMS_CPP
#define PARAMS_CPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>

using namespace std;

class params{
	int params_count;
	public:
		//separador em arquivos csv
		static char sep;
		
		//flag para execucao em modo de criacao de conjuntos
		static char FLAG_CREATE_SETS[2];
		
		//parametros de deteccao de nikolov
		static double gama;
		static double theta;
		static int detectionsLimit;
		
		//tamanho da suavizacao (quantidade de amostras)
		static int NSMOOTH;
		
		//parametros nikolov.hpp
		//tamanho do sinal de referencia
		static int NREF;
		//tamanho do sinal de observacao
		static int NOBS;
		//tamanho da tendencia
		static int NTREND;
		//porcentagem do treshold
		static double NTRESH;

		//medias moveis longa e curta
		static int sma_period;
		static int lma_period;
		
		//nomes dos arquivos de treinamento e teste
		static string trFN;
		static string teFN;
		
		void print();
		std::string header_csv();
		std::string to_string_csv();
		bool initialize( int argc, char *argv[] );
		void set_params( float g, float t, int dl, int nsm, int nre, int nob, int smap, int lmap, string trfn, string tefn );
};

#endif
