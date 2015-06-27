#include "simulator.hpp"


float simulator::run( vector<float> source, vector<int> decision ){
	
	float priceIn = 0, priceOut = 0; //preco de entrada e saida
	float priceTax = 0; //taxas da operacao
	int lastOp = 0; //ultima operacao: 1=compra, -1=venda, 0=nenhuma
	float accumProfit = 1; //lucro acumulado
	funds = init_funds;

	ofstream f_profit;
	f_profit.open( funds_filename );
	for( int i = 0; i < decision.size(); i++ ){
		if( decision[i] != 0 ){
			profit = 0;
			priceOut = source[i];
			priceTax = taxes( source[i] );
			if( decision[i] == 1 ){//compra
				priceOut += priceTax;
				if( lastOp == -1 ){//fecha venda
					profit = (priceIn-priceOut)/priceIn;
					//~ profit = priceIn-priceOut;
				}
				lastOp = 1;
			} else { //venda
				priceOut -= priceTax;
				if( lastOp == 1 ){//fecha a compra
					profit = (priceOut-priceIn)/priceIn;
					//~ profit = priceOut-priceIn;
				}
				lastOp = -1;
			}
			
			//~ if( profit >= 0 )
			if( profit >= 0 )
				pos_trades++;
			else 
				neg_trades++;
			//~ funds += (profit*order_size)-taxes;
			//~ funds *= (1 + profit);
			accumProfit *= (1 + profit);
			
			priceIn = priceOut;
		}
		
		f_profit << accumProfit << endl;
	}
	f_profit.close();
	
	pos_trades--; //bug fix: entrada do primeiro trade esta contando pos_trade++
	
	//~ +Trades,-Trades,Acc,InitS,FinalS,Return
	//~ float accuracy = (float)pos_trades/((float)pos_trades+(float)neg_trades)*100;
	//~ float ret = ((funds/init_funds)-1)*100;
	//~ cout << pos_trades << " " << neg_trades << " " << accuracy << "% " << init_funds << " " <<funds << " " << ret << "%" << endl;
	
	//~ +Trades,-Trades,Acc,Return
	float accuracy = (float)pos_trades/((float)pos_trades+(float)neg_trades);
	profit = (accumProfit-1);
	//~ cout << pos_trades << " " << neg_trades << " " << accuracy << " " << profit << endl;
	return profit;
}

int simulator::pTrades(){
	return pos_trades;
}

int simulator::nTrades(){
	return neg_trades;
}

int simulator::finalProfit(){
	return profit;
}

void simulator::set_init_funds( float init ){
	init_funds = init;
}

void simulator::set_taxes( float _tax ){
	tax = _tax;
}

float simulator::taxes( float volume ){
	return volume * tax;
}

void simulator::set_order_size( int order_sz ){
	if( order_sz > 0 )
		order_size = order_sz; //quantidade de acoes a comprar
	else 
		order_size = -1; //usar sempre o maximo possivel
}

