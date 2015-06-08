#include "simulator.hpp"


void simulator::run( vector<float> source, vector<int> decision ){
	
	float priceIn = 0, priceOut = 0; //preco de entrada e saida
	int lastOp = 0; //ultima operacao: 1=compra, -1=venda, 0=nenhuma
	funds = init_funds;

	ofstream f_profit;
	f_profit.open( funds_filename );
	for( int i = 0; i < decision.size(); i++ ){
		if( decision[i] != 0 ){
			profit = 0;
			priceOut = source[i];
			if( decision[i] == 1 ){//compra
				//fecha venda
				if( lastOp == -1 ){
					profit = priceIn-priceOut;
				}
				lastOp = 1;
			} else { //venda
				if( lastOp == 1 ){
					profit = priceOut-priceIn;
				}
				lastOp = -1;
			}
			
			if( profit >= 0 )
				pos_trades++;
			else 
				neg_trades++;
			funds += (profit*order_size)-taxes;
			
			priceIn = priceOut;
		}
		
		if( i+1 == decision.size() && lastOp != 0 ){ //ultimo valor, fecha operacao em aberto
			priceOut = source[i];
			if( lastOp == 1 ){
				profit = priceOut-priceIn;
			} else if( lastOp == -1 ){
				profit = priceIn-priceOut;
			}
				
			if( profit >= 0 )
				pos_trades++;
			else 
				neg_trades++;
			funds += (profit*order_size)-taxes;
		}
		f_profit << funds << endl;
	}
	f_profit.close();
	
	//~ +Trades,-Trades,Acc,InitS,FinalS,Return
	float accuracy = (float)pos_trades/((float)pos_trades+(float)neg_trades)*100;
	float ret = ((funds/init_funds)-1)*100;
	cout << pos_trades << " " << neg_trades << " " << accuracy << "% " << init_funds << " " <<funds << " " << ret;
	
	//~ cout << "Positive trades: " << pos_trades << endl;
	//~ cout << "Negative trades: " << neg_trades << endl;
	//~ cout << "Accuracy: " << (float)pos_trades/((float)pos_trades+(float)neg_trades)*100 << "%" << endl;
	//~ cout << "Initial funds: " << init_funds << endl;
	//~ cout << "Final funds: " << funds << endl;
	//~ cout << "Return: " << ((funds/init_funds)-1)*100 << "%" << endl;
}


void simulator::set_init_funds( float init ){
	init_funds = init;
}

void simulator::set_taxes( float tax ){
	taxes = tax;
}

void simulator::set_order_size( int order_sz ){
	if( order_sz > 0 )
		order_size = order_sz; //quantidade de acoes a comprar
	else 
		order_size = -1; //usar sempre o maximo possivel
}
