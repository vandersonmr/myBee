#include "dataDAO.hpp"
#include <stdio.h>
#include "limit.h"
#include "KCentroid.h"
#include "Node.h"
#include <unordered_map>
#include <memory>

using namespace std;

unordered_map<string,unique_ptr<Node>> Nodes;

int testData(Data<double> data, const DataDAO<double>& dao){
  int resLimit = testLimit(data);
  string nodeName = string(data.nickname+data.type);

  if(Nodes[nodeName] == nullptr)
    Nodes[nodeName] = unique_ptr<Node>(new Node(dao));

  //int resKCentroid = Nodes[nodeName]->kcentroidControl.testData(data,data.nickname);
  //int resKrls = Nodes[nodeName]->krlsControl.testData (data,data.nickname);

  return resLimit ;//| resKCentroid | resKrls;
}
