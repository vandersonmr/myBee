#ifndef KRLS_H
#define KRLS_H
#include "ManipulateData.h"


class Krls: public ManipulateData{
	protected:
		krls<kernel_type> krlsTest;
		void insertData(Data);
		int estimate(Data);
	
	public:
		Krls();
		~Krls(){}
};


#endif
