#include "dataDAO.h"
#include <stdio.h>
#include "include/limit.h"
#include "include/KCentroid.h"
#include "include/Krls.h"
#include "include/Rvm.h"

using namespace std;

int testData(Data* data){
	int resLimit = testLimit(data);
	int resKCentroid = KCentroid::kcentroidControl.testData(data);	
	int resKrls = Krls::krlsControl.testData(data);
	int resRvm = Rvm::rvmControl.testData(data);
	return resLimit | resKCentroid | resKrls | resRvm;
}
