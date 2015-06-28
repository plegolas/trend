#ifndef MYTIME_CPP
#define MYTIME_CPP

#include <ctime>
#include <string>
#include <cstdio>

using namespace std;

time_t str2time_t (const char *str);
time_t string2time_t ( string str );

void time_t2str (char *str, time_t t);
string time_t2string ( time_t t );

void get_prices( time_t *date, float *price, const char *line );
void get_prices( time_t *date, float *price, string line );

void get_prices_yahoo( time_t *date, float *price, const char *line );
void get_prices_yahoo( time_t *date, float *price, string line );
void get_prices_yahoo( float *price, string line );

#endif
