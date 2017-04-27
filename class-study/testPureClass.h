#ifndef _TEST_PURECLASS_H_
#define _TEST_PURECLASS_H_

class PureClass {
	public:
	PureClass();
	~PureClass();
	int m_mutuble;
};

class DeClass:public PureClass {
	public:
		DeClass();
		~DeClass();
};


void TestPureClass ();

#endif