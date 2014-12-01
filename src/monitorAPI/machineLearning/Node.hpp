#ifndef NODE_H
#define NODE_H
#include "ManipulateData.hpp"
#include "Krls.hpp"
#include "KCentroid.hpp"
#include "../database/dataDAO.hpp"

template <typename T>
class Node {
  public:
    Node() {}
    Node(const DataDAO<T>& dao) {
      krlsControl.setDAO(dao);
      kcentroidControl.setDAO(dao);
    }
    Krls<T> krlsControl;
    KCentroid<T> kcentroidControl;
};

#endif
