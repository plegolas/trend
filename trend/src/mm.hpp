#ifndef MM_CPP
#define MM_CPP

#include <vector>

class mm{
	
	private:
		std::vector<float> values;
		bool is_full = false;
		int oldest_value;
	
	public:
		//media dos valores
		float average();
		
		int size();
		
		void set_size( int size );
		
		void init( std::vector<float> values_ );
		
		void add( float value );
};

#endif

