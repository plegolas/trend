#ifndef MM_CPP
#define MM_CPP

#include <vector>

class mm{
	
	private:
		//quantidade de valores
		int period;
		
		std::vector<float> values;
	
	public:
		//media dos valores
		float average();
		
		int get_period();
		
		void init( std::vector<float> values_ );
		
		void add( float value );
};

#endif

