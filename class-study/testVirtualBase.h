#ifndef _TEST_VIRTUAL_BASE_
#define _TEST_VIRTUAL_BASE_

#include "print.h"

class Base {
	public:
		int m_value;
};

class DerivedA : public virtual Base {

};

class DerivedB : public virtual Base {

};

class DerivedC: public DerivedA, public DerivedB {

};

void TestVirtualBase();

#endif