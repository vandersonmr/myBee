#ifndef GENERIC_DATA
#define GENERIC_DATA

#include <string>
#include <sstream>

class Generic {
  private:
    std::string name;
    int type;
    double temperature, humidity, pressure;
    std::string command;

  public:
    Generic() {};

    Generic(std::string name) {
      this->name = name;
    };

    void setName(std::string name) {
      this->name = name;
    };

    std::string getName() {
      return this->name;
    };

    void setType(int t) { type = t; };

    int getType() { return type; };
    
    void setCommand(std::string c) { command = c; };
    
    std::string getCommand() { return command; };

    double getTemperature() { return temperature; };

    void setTemperature(double t) { temperature = t; };

    double getHumidity() { return humidity; };

    void setHumidity(double h) { humidity = h; };

    double getPressure() { return pressure; };

    void setPressure(double p) { pressure = p; };

    MSGPACK_DEFINE(name, type, temperature, pressure, humidity, command);
};

//this overload must be implemented to persist data on database
std::stringstream& operator<<(std::stringstream& ss, Generic& g) {
  if (g.getType() == 0)
    ss << g.getTemperature();
  else if (g.getType() == 1)
    ss << g.getHumidity();
  else if (g.getType() == 2)
    ss << g.getPressure();
  else if (g.getType() == 3)
    ss << g.getCommand();
  else
    ss << g.getName();
  return ss;
};

std::ostream& operator<<(std::ostream& os, Generic& g) {
  if (g.getType() == 0)
    os << g.getTemperature();
  else if (g.getType() == 1)
    os << g.getHumidity();
  else if (g.getType() == 2)
    os << g.getPressure();
  else if (g.getType() == 3)
    os << g.getCommand();
  else
    os << g.getName();
  return os;
};

#endif
