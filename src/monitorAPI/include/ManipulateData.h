#ifndef MANIPULATE_H
#define MANIPULATE_H

#include <iostream>
#include <vector>
#include <dlib/svm.h>
#include <dlib/statistics.h>
#include "dataDAO.hpp"
#include <string>

using namespace std;
using namespace dlib;

#define ACCURACY 0.1
#define sizeOfKnow 1000
#define LIMIT 5

class ManipulateData{
  protected:
    DataDAO<double> dao;
    typedef matrix<double,1,1> sample_type; // 1x1 [temp]
    typedef radial_basis_kernel<sample_type> kernel_type;

    bool load;
    void checkLoad(string, string);
    void loadData(string , string);
    virtual void fillData(std::vector<Data<double>>);
    virtual void insertData(Data<double>){}
    virtual int estimate(Data<double>){ return 0;}

  public:
    ManipulateData(const DataDAO<double>& dao) { this->dao = dao; load = false;}
    virtual ~ManipulateData(){}
    int testData(Data<double>, string);
};


#endif
