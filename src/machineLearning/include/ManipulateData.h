#ifndef MANIPULATE_H
#define MANIPULATE_H

#include <iostream>
#include <vector>
#include <stdio.h>

#include <dlib/svm.h>
#include <dlib/statistics.h>
#include "dataDAO.h"

using namespace std;
using namespace dlib;

#define ACCURACY 0.1
#define sizeOfKnow 1000

class ManipulateData{
	protected:
		typedef matrix<double,1,1> sample_type; // 1x1 [temp]
		typedef radial_basis_kernel<sample_type> kernel_type;
		bool load;
		void checkLoad();
		void loadData();
		virtual void insertData(Data*){} 
		virtual int estimate(Data*){ return 0;}
	
	public:	
		ManipulateData(){ load = false;}
		virtual ~ManipulateData(){}
		int testData(Data*);
};


#endif
