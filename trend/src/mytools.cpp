#include "mytools.hpp"
#include <iostream>

time_t str2time_t (const char *str){
	int y, m, d;
	if( str[2] == '/' )
		sscanf(str, "%d/%d/%d", &d, &m, &y);
	else
		sscanf(str, "%d-%d-%d", &y, &m, &d);

	struct tm when = {0};
	when.tm_year = y-1900;
	when.tm_mon = m-1;
	when.tm_mday = d;

	time_t converted;
	converted = mktime(&when);

	return converted;
}

time_t string2time_t( string str ){
	return str2time_t( str.c_str() );
}

void time_t2str (char *str, time_t t){
	struct tm *t2;
	t2 = localtime(&t);

	int day, mon, year;
	day = t2->tm_mday;
	mon = t2->tm_mon + 1;
	year = t2->tm_year + 1900;

	sprintf(str, "%d/%d/%d", day, mon, year);
}

string time_t2string ( time_t t ){
	char tm[18];
	time_t2str( tm, t );
	string str (tm);
	return str;
}

//Get date and stock price in a line with the folowing pattern:
//Data;Variação;Abertura;Máximo;Mínimo;Fechamento;Negócios;Quantidade;Volume
//13/06/2014;0.58;18.63;19.07;18.61;18.99;35,365;28,004,000;529,200,928
//Data from InvestCharts
void get_prices( time_t *date, float *price, const char *line ){
	char str_date[18];
	sscanf(line, "%[^;];%*[^;];%*[^;];%*[^;];%*[^;];%f;", str_date, price);
	*date = str2time_t( str_date );
}

void get_prices( time_t *date, float *price, string line ){
	return get_prices( date, price, line.c_str() );
}

//Get date and stock price in a line with the folowing pattern:
//Date,Open,High,Low,Close,Volume,Adj Close
//2014-07-08,17.36,17.42,17.17,17.33,11504900,17.33
//Data from Yahoo Finance
void get_prices_yahoo( time_t *date, float *price, const char *line ){
	char str_date[18];
	sscanf(line, "%[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%f", str_date, price);
	*date = str2time_t( str_date );
}

void get_prices_yahoo( time_t *date, float *price, string line ){
	return get_prices_yahoo( date, price, line.c_str() );
}

void get_prices_yahoo( float *price, string line ){
    time_t date_discart;
	get_prices_yahoo( &date_discart, price, line.c_str() );
}

