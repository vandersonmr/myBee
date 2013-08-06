#include <dataDAO.h>

using namespace std;

#define UPPER_BOUND 39
#define LOWER_BOUND 20

int testLimit(Data* data){
	double temp = data->temperature;
	if(temp < UPPER_BOUND && temp > LOWER_BOUND){
		return 0;
	}
	return 1;	
}
