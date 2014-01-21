#include <dataDAO.hpp>

using namespace std;

#define UPPER_BOUND 40
#define LOWER_BOUND 10

int testLimit(Data data){
	double temp = data.value;
	if(temp < UPPER_BOUND && temp > LOWER_BOUND){
		return 0;
	}
	return 1;	
}
