#include "include/ManipulateData.h"

void ManipulateData::checkLoad(char* nodeName){
	if(!load){
		load = true;
		loadData(nodeName);
	}
}

void ManipulateData::loadData(char* nodeName){
	Data* data;
	int numrows = loadLastsDatas(&data,sizeOfKnow,nodeName);
	fillData(data,numrows);
}

void ManipulateData::fillData(Data* data, int numrows){
	for(int i=0;i<numrows;i++){
		insertData(&data[i]);
	}
}

int ManipulateData::testData(Data* data, char* nodeName){
	checkLoad(nodeName);
	return estimate(data);
}
