#include "../include/dataDAO.hpp"
#include <stdio.h>
#include "include/limit.h"
#include "include/KCentroid.h"
#include "include/Node.h"
#include <unordered_map>
#include <memory>

using namespace std;


unordered_map<string,unique_ptr<Node>> Nodes;

int testData(Data data){
  int resLimit = testLimit(data);
  string nodeName = string(data.nickname+data.type);

  if(Nodes[nodeName] == nullptr)
    Nodes[nodeName] = unique_ptr<Node>(new Node());

  int resKCentroid = Nodes[nodeName]->kcentroidControl.testData(data,data.nickname);
  int resKrls      = Nodes[nodeName]->krlsControl.testData (data,data.nickname);

  return resLimit | resKCentroid | resKrls;
}
