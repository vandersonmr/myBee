#ifndef NODE_H
#define NODE_H
#include "ManipulateData.h"

#include "Krls.h"
#include "KCentroid.h"


class Node {
  public:
    Krls krlsControl;
    KCentroid kcentroidControl;

    Node(const DataDAO<double>& dao) 
      : krlsControl(dao), kcentroidControl(dao) {};

};

#endif
