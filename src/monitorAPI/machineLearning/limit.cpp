#include <../include/dataDAO.hpp>

using namespace std;

#define UPPER_BOUND 36 // (Winston 1987, Tautz, 2008).
#define LOWER_BOUND 31

#define UPPER_HUMIDITY 80 // http://www.arnia.co.uk/hive-humidity/
#define LOWER_HUMIDITY 38

#define UPPER_PRESSURE 10 //kPa
#define LOWER_PRESSURE 5

int testLimit(Data data){
  double value = data.value;
  if(data.definedType.sensor != Type::None) { 
    switch(data.definedType.sensor) {
      case Type::Humidity:
        if(value < UPPER_HUMIDITY && value > LOWER_HUMIDITY)
          return 0;
        return 1;
      case Type::Temperature:
        if(value < UPPER_BOUND && value > LOWER_BOUND)
          return 0;
        return 1;
      default:
        return 1;
    }
  }
  return 1;	
}
