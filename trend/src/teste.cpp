#include <iostream>
#include <fstream>

using namespace std;

int main(){
	
	//~ std::filebuf flog ;
    //~ flog.open( "log.csv", std::ios::out ) ;
    //~ std::clog.rdbuf( &flog ) ;
	//~ 
	cout << "mensagem cout\n";
	cerr << "mensagem cerr\n";
	clog << "mensagem clog\n";
}
