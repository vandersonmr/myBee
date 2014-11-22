#ifndef MANIPULATE_H
#define MANIPULATE_H

#include <iostream>
#include <vector>
#include <dlib/svm.h>
#include <dlib/statistics.h>
#include "../database/dataDAO.hpp"
#include <string>

using namespace std;
using namespace dlib;

#define ACCURACY 0.1
#define sizeOfKnow 1000
#define LIMIT 5

template <typename T>
class ManipulateData {
  protected:
    DataDAO<T> dao;
    typedef matrix<T,1,1> sample_type; // 1x1 [temp]
    typedef radial_basis_kernel<sample_type> kernel_type;

    bool load;
    void checkLoad(string, string);
    void loadData(string, string);
    virtual void fillData(std::vector<Data<T>>);
    virtual void insertData(Data<T>){} 
    virtual int estimate(Data<T>){ return 0;}

  public:	
    ManipulateData(){ load = false;}
    virtual ~ManipulateData(){}
    int testData(Data<T>, string);
    void setDAO(const DataDAO<T>& dao) {
      this->dao = dao;
    }
};

template <typename T>
void ManipulateData<T>::checkLoad(string nodeName, string type){
  if(!load){
    load = true;
    loadData(nodeName, type);
  }
}

template <typename T>
void ManipulateData<T>::loadData(string nodeName, string type){
  std::vector<Data<T>> datas = dao.loadLastsDatasByType(sizeOfKnow,nodeName,type);
  fillData(datas);
}

template <typename T>
void ManipulateData<T>::fillData(std::vector<Data<T>> datas){
  for (Data<T> data : datas)
    insertData(data);
}

template <typename T>
int ManipulateData<T>::testData(Data<T> data, string nodeName){
  checkLoad(nodeName, data.type);
  return estimate(data);
}

#endif
