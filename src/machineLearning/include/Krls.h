#ifndef KRLS_H
#define KRLS_H
#include "ManipulateData.h"

class Krls: public ManipulateData{
	protected:
		krls<kernel_type> krlsTest;
		double convertTimeToSeconds(char*);
		void insertData(Data*);
		int estimate(Data*);
	
	public:
		Krls();
		~Krls(){}
		static Krls krlsControl;
};


#endif
