#ifndef MACHINELEARNING_H
#define MACHINELEARNING_H
#include "../database/dataDAO.hpp"
#include <stdio.h>
#include "limit.hpp"
#include "KCentroid.hpp"
#include "Node.hpp"
#include <unordered_map>
#include <memory>

using namespace std;

template <typename T>
class MachineLearning {
  private:
    Limit<T> lim;
    DataDAO<T> dao;
    unordered_map<string,unique_ptr<Node<T>>> Nodes;

  public:
    MachineLearning() {};
    int testData(Data<T>);
    void setDAO(const DataDAO<T>& dao) {
      this->dao = dao;
    }
};

template <typename T>
int MachineLearning<T>::testData(Data<T> data){
  int resLimit = lim.testLimit(data);
  string nodeName = string(data.nickname+data.type);

  if(Nodes[nodeName] == nullptr)
    Nodes[nodeName] = unique_ptr<Node<T>>(new Node<T>(dao));

  int resKCentroid = Nodes[nodeName]->kcentroidControl.testData(data,data.nickname);
  int resKrls      = Nodes[nodeName]->krlsControl.testData (data,data.nickname);

  return resLimit | resKCentroid | resKrls;
}

#endif
