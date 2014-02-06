#include <dataDAO.hpp>

using namespace std;

#define UPPER_BOUND 40
#define LOWER_BOUND 10

#define UPPER_HUMIDITY 80
#define LOWER_HUMIDITY 40

#define UPPER_PRESSURE 10 //kPa
#define LOWER_PRESSURE 5

int testLimit(Data data){
    double temp = data.type_value["temperature"];
    if(temp < UPPER_BOUND && temp > LOWER_BOUND){
        return 0;
    }
    return 1;	
}
