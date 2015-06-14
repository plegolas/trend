#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class simulator{
	const char *funds_filename = "00funds";
	int pos_trades = 0, neg_trades = 0;
	float init_funds = 0, funds;
	float tax = 0; //taxas por operacao
	int order_size = 100; //quantidade de ativos por operacao. -1 = maximo
	float profit = 0;
	
	float taxes( float volume );
	
	public:
	simulator(){};
	~simulator(){};
	void run( vector<float> source, vector<int> decision );
	void set_init_funds( float init );
	void set_taxes( float _tax );
	void set_order_size( int order_sz );

};
#endif
