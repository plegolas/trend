#include "set_util.hpp"


void set_util::print(string filename, vector<signal> *r){
	filename += (char) 'A';
	for( int t = 0; t <= 5; t++){
		for( int j = 0; j < r->size(); j++ ){
			switch (t){
				case 1: 
					(*r)[j].baseline_normalization();
					break;
				case 2: 
					(*r)[j].spike_normalization();
					break;
				case 3:  
					(*r)[j].smoothing();
					break;
				case 4:  
					(*r)[j].log_scale();
					break;
				case 5:  
					(*r)[j].pop_front( params::NSMOOTH );
					break;
			}
		}
		
		ofstream outfile (filename);
		int len = (*r)[0].lenght();
		for( int i = 0; i < len; i++ ){
			for( int j = 0; j < r->size(); j++ ){
				if( j == 0 )
					outfile << (*r)[j].at(i);
				else
					outfile << "," << (*r)[j].at(i);
			}
			outfile << "\n";
		}
		outfile.close();
		filename[ filename.size() -1 ] = (char) (t+'B');
	}
	
	clog << "Set " << filename << " written" << endl;
}


void set_util::load( vector<signal> *r, string filename ){
	filename += "A";
	ifstream infile;
	infile.open( filename );
	if( !infile.good() ){
		cerr << "file " << filename << " not opened" << endl;
		exit( 1 );
	}
	
	string line;
	
	getline( infile, line );
	init( r, line );
	
	do{
		update( r, line );
		getline( infile, line );
	} while( !infile.eof() );

	infile.close();
	
	clog << "Set " << filename << " loaded " << r->size() << " items" << endl;
}

void set_util::update( vector<signal> *r, string line ){
	//cria vetor do tamanho das quantidades de valores na linha
	vector<float> values;
	values.clear();
	int sz = count( line.begin(), line.end(), ',' ) + 1;
	values.resize( sz );
	
	char *cline = (char*) line.c_str();
	const char *pch = strtok( cline, "," );
	int i = 0;
	while( pch != NULL ){
		float v;
		sscanf( pch, "%f", &v );
		values[i++] = v;
		pch = strtok( NULL, "," );
	}
	
	for( i = 0; i < r->size(); i++ ){
		(*r)[i].add( values[i] );
	}
}

void set_util::transform( vector<signal> *set ){
	vector<signal>::iterator it = set->begin();
	for( ; it != set->end(); it++ ){
		it->transform();
	}
}

void set_util::init( vector<signal> *r, string line ){
	int sz = count( line.begin(), line.end(), ',' );
	sz++;
	
	r->clear();
	r->resize( sz );
	clog << "Set initialized" << endl;
}
