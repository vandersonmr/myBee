#ifndef KCENTROID_H
#define KCENTROID_H
#include "ManipulateData.h"

#define limit 1

class KCentroid: public ManipulateData {
	protected:
		kcentroid<kernel_type> kcentroidTest;
		void insertData(Data);
		int estimate(Data);
	
	public:
		KCentroid();
		~KCentroid(){}
};

#endif
