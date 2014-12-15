#ifndef DETECTION_HPP
#define DETECTION_HPP

#include "signal.hpp"

class detection{
	public:
		signal sig;
		time_t time;
		signal after_detection;
		float first;
		float last;
		float max;
};

#endif
