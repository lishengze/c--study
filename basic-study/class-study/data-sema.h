#ifndef _DATA_SEMA_H_
#define _DATA_SEMA_H_


class A {
	
};

class B: public virtual A {

};

class C: public virtual A {

};

class D: public B, public C {

};

void TestEmptyClassSize();




#endif