#ifndef KRLS_H
#define KRLS_H
#include "ManipulateData.hpp"
#include <time.h>

template <typename T>
class Krls: public ManipulateData<T>{
  protected:
    typedef typename ManipulateData<T>::sample_type sample_type;
    typedef typename ManipulateData<T>::kernel_type kernel_type;
    krls<kernel_type> krlsTest;
    void insertData(Data<T>);
    int estimate(Data<T>);
    int getTime(Data<T> data);

  public:
    Krls();
    ~Krls(){}
};

template <typename T>
Krls<T>::Krls() : ManipulateData<T>(), 
  krlsTest(kernel_type(ACCURACY), 0.0001) {}

template <typename T>
int Krls<T>::getTime(Data<T> data) {
  auto time = gmtime(&data.time); 
  return time->tm_hour*60 + time->tm_min; 
}

template <typename T>
void Krls<T>::insertData(Data<T> data){
  sample_type m;
  m(0) = getTime(data);
  krlsTest.train(m, data.value);
}

template <typename T>
int Krls<T>::estimate(Data<T> data){
  sample_type m;
  m(0) = getTime(data);
  cout << "Krls -> " << data.type << " esperada: " << krlsTest(m) << endl;
  cout << "Krls -> " << data.type << " real: " << data.value << endl;
  int status = abs(krlsTest(m) - (T) data.value) > LIMIT? 4 : 0;
  insertData(data);
  return status;
}

#endif
