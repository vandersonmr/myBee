#ifndef KRLS_H
#define KRLS_H
#include "ManipulateData.h"
#include <time.h>

class Krls: public ManipulateData{
  protected:
    krls<kernel_type> krlsTest;
    void insertData(Data<double>);
    int estimate(Data<double>);

  public:
    Krls(const DataDAO<double>&);
    ~Krls(){}
};


#endif
