#include "testStaticCast.h"
#include "print.h"

void base1::func() {
	cout << "base::func() " << endl;
}

void derived1::func() {
	cout << "derived1::func() " << endl;
}

void derived1::funcd() {
	cout << "derived1::funcd() " << endl;
}


void derived2::func() {
	cout << "derived2::func() " << endl;
}

void derived2::funcd() {
	cout << "derived2::funcd() " << endl;
}

void derived3::func() {
	cout << "derived3::func() " << endl;
}

void derived3::funcd() {
	cout << "derived3::funcd() " << endl;
}



void testStaticCast() {
	// base1 b1;
	// derived1 d1;
	// b1 = static_cast<base1>(d1);
	// d1 = static_cast<derived1>(b1);
	// cout << "testStaticCast " << endl;
	// int a = 10;
	// double b = 10.1;
	// b = static_cast<double>(a);

	base1* b1;
	derived1 d1;
	derived1* pd1;
	derived2 d2;
	derived2* pd2 = &d2;
	derived3 d3;
	derived3* pd3;

	// b1 = static_cast<derived1*>(&d1);	
	b1 = &d2;
	if (!(pd1 = dynamic_cast<derived1*>(b1))) {
		cout << "pd1 = dynamic_cast<derived1*>(b1) failed!" << endl; 
	} else {
		pd1 -> func();
	}
	pd2 = static_cast<base1*>(pd3);

	// pd1 = static_cast<derived1*>(b1);
	// pd1 -> func();
	// b1->funcd();
}