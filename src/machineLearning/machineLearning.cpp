#include "dataDAO.h"
#include <stdio.h>
#include "include/limit.h"
#include "include/kcentroid.h"
using namespace std;

int testData(Data* data){
	int resLimit = testLimit(data);
	int resKCentroid = testKCentroid(data);	
	return resLimit | resKCentroid;
}
