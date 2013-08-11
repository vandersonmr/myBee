#ifndef NODE_H
#define NODE_H
#include "ManipulateData.h"

#include "Rvm.h"
#include "Krls.h"
#include "KCentroid.h"


class Node {
	public:
           Krls krlsControl;
           Rvm rvmControl;
           KCentroid kcentroidControl;
};

#endif
