/*
   The kcentroid object is an implementation of an algorithm that recursively
   computes the centroid (i.e. average) of a set of points.

   For another example: http://dlib.net/kcentroid_ex.cpp.html
   */

#include "include/KCentroid.h"


KCentroid::KCentroid() : kcentroidTest(kernel_type(ACCURACY),0.005, 30), ManipulateData(){}

void KCentroid::insertData(Data data){
	sample_type m;
	m(0) = data.value;
	kcentroidTest.train(m);
}

int KCentroid::estimate(Data data){
	sample_type m;
	m(0) = data.value;
	return kcentroidTest(m) > limit? 2 : 0;
}
