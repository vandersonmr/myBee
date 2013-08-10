#ifndef RVM_H
#define RVM_H

#include "ManipulateData.h"

#define AMOUNT 10

class Rvm: public ManipulateData{
	protected:
		double gamma;
		std::vector<sample_type> samples;
		std::vector<double> labels;
		rvm_regression_trainer<kernel_type> rvmTest;
		decision_function<kernel_type> rvmFunction;
		void fillData(Data*,int);
		void insertData(Data*);
		void trainData();
		void pushNewData(Data*, sample_type);
		int estimate(Data*);
	
	public:
		Rvm();
		~Rvm(){}
		static Rvm rvmControl;

};

#endif
