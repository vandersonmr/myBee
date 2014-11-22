#ifndef GENERIC_DATA
#define GENERIC_DATA

#include <string>
#include <sstream>

class Generic {
  private:
    std::string name;
    uint16_t size;

  public:
    Generic() {};

    Generic(std::string name, uint16_t size) {
      this->name = name;
      this->size = size;
    };

    void setName(std::string name) {
      this->name = name;
    };

    std::string getName() {
      return this->name;
    };

    void setSize(uint16_t size) {
      this->size = size;
    };

    uint16_t getSize() {
      return this->size;
    };
    MSGPACK_DEFINE(name, size);
};

#endif
