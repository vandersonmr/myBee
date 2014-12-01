#include "ManipulateData.h"

void ManipulateData::checkLoad(string nodeName, string type){
  if(!load){
    load = true;
    loadData(nodeName, type);
  }
}

void ManipulateData::loadData(string nodeName, string type){
  std::vector<Data<double> > datas 
    = dao.loadLastsDatasByType(sizeOfKnow, nodeName, type);
  fillData(datas);
}

void ManipulateData::fillData(std::vector<Data<double>> datas){
  for (Data<double> data : datas)
    insertData(data);
}

int ManipulateData::testData(Data<double> data, string nodeName){
  checkLoad(nodeName, data.type);
  return estimate(data);
}
