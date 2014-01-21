#ifndef RVM_H
#define RVM_H

#include <vector>
#include "ManipulateData.h"

#define AMOUNT 20

class Rvm: public ManipulateData{
	protected:
		double gamma;
		std::vector<sample_type> samples;
		std::vector<double> labels;
		rvm_regression_trainer<kernel_type> rvmTest;
		decision_function<kernel_type> rvmFunction;
		void fillData(std::vector<Data>,int);
		void insertData(Data);
		void trainData();
		void pushNewData(Data, sample_type);
		int estimate(Data);
	
	public:
		Rvm();
		~Rvm(){}

};

#endif
