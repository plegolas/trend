#include "signal.hpp"

signal::signal(){ 
	serie.clear(); 
	transformed = false;
}

signal::signal( std::vector<float> s ){ 
	serie.assign( s.begin(), s.end() ); 
	transformed = false;
}
	
signal::~signal(){ 
	serie.clear(); 
}

int signal::lenght(){ 
	return serie.size(); 
}

bool signal::is_transformed(){ 
	return transformed; 
}

void signal::set_transformed( bool t ){
	transformed = t; 
}

void signal::print(){
	SIGNALIT it = serie.begin();
	while( it != serie.end() ){
		cout << *it << " "; 
		it++;
	}
	cout << endl;
}

void signal::print( string outfile ){
	ofstream out (outfile);
	SIGNALIT it = serie.begin();
	while( it != serie.end() ){
		out << *it << endl; 
		it++;
	}
	//~ out << endl;
	out.close();
}

float signal::at( int pos ){ 
	return serie.at(pos); 
}

void signal::add( float value ){ 
	serie.push_back(value); 
}

float signal::trend_return( int trend_size ){
	int sz = serie.size();
	return (serie[sz-1] / serie[sz-trend_size])-1.0;
}

//transforacao da serie temporal em sinal
//pode-se testar outras formas de transformacao alterando o switch
void signal::transform(){
	if( is_transformed() )
		return;
	
	int i = 2;
	switch( i ){
		case 0: 
			accum_log();
			break;
		case 1: 
			simple_returns();
			break;
		case 2:
			nikos_signal();
			break;
	}
	transformed = true;
}

//soma dos retornos simples
void signal::simple_returns(){
	SIGNALIT itNew, itOld;
	itNew = itOld = serie.end();
	itOld--;
	while( itOld != serie.begin() ){
		itNew--; itOld--;
		*itNew = (*itNew / *itOld)-1;
	}
	*itOld = 0;
}


//soma dos logs dos retornos
void signal::accum_log(){
	SIGNALIT itNew, itOld;
	itNew = itOld = serie.begin();
	itNew++;
	float sum = 0;
	
	for( ; itNew != serie.end(); itNew++, itOld++ ){
		float aux = log(*itNew / *itOld);
		*itOld = sum;
		sum += aux;
	}
	*itOld = sum;
}

//transformacao sugerida por nikolov
void signal::nikos_signal(){
	baseline_normalization();
	spike_normalization();
	smoothing();
	log_scale();
}

void signal::baseline_normalization(){
	float b = 0; //soma dos elementos da serie
	for( int i = 0; i < serie.size(); i++ ){
		b += serie[i];
	}
	
	int beta = 1; //nikolov usou este beta fixo em 1
	for( int i = 0; i < serie.size(); i++ ){
		serie[i] = (serie[i]/b); //TODO: verificar divisao por zero
		//~ serie[i] = pow( serie[i]/sum, beta ); //TODO: verificar divisao por zero
	}
}

void signal::spike_normalization(){
	float alpha = 1.2; //nikolov usou alpha fixo em 1.2
	//~ for( int i = 1; i < serie.size(); i++ ){
	for( int i = serie.size()-1; i > 0; i-- ){
		serie[i] = serie[i] - serie[i-1];
		if( serie[i] < 0 ) serie[i] *= -1;
		serie[i] = pow( serie[i], alpha );
	}
	serie[0] = serie[1]; //bug fix
}

void signal::smoothing(){
	SIGNALTYPE copy (serie);
	float sum = 0;
	int i, j;
	i = j = 0;
	for( ; i < serie.size(); i++ ){
		sum += copy[i];
		if( i == j+params::NSMOOTH ){
			sum -= copy[j];
			j++;
		}
		serie[i] = sum;
	}
}

void signal::log_scale(){
	for( int i = 0; i < serie.size(); i++ ){
		if( serie[i] == 0 ) serie[i] = 0.01; //TODO: verificar log de zero
		serie[i] = log(serie[i]);
	}
}


void signal::reduce( int size ){
	serie.assign( serie.begin(), serie.end()-size );
}

void signal::pop_front( int size ){
	serie.assign( serie.begin()+size, serie.end() );
}

signal signal::subSignal( int begin, int size ){
	std::vector<float> vec( &(serie.at(begin)), &(serie.at(begin))+size );
	signal sig( vec );
	sig.set_transformed( is_transformed() );
	return sig;
}
