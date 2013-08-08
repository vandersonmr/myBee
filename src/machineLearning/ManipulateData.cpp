#include "include/ManipulateData.h"

void ManipulateData::checkLoad(){
	if(!load){
		loadData();
		load = true;
	}
}

void ManipulateData::loadData(){
	Data* data = (Data*) malloc(sizeof(Data)*sizeOfKnow);
	int numrows = loadLastsDatas(data,sizeOfKnow);
	for(int i=0;i<numrows;i++){
		insertData(&data[i]);
	}
}

int ManipulateData::testData(Data* data){
	checkLoad();
	return estimate(data);
}
