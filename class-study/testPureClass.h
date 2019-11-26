#ifndef _TEST_PURECLASS_H_
#define _TEST_PURECLASS_H_

class PureClass {
	public:
	PureClass();
	virtual void test() = 0;
	virtual ~PureClass();
	// int m_mutuble;
};

class DeClass:public PureClass {
	public:
		DeClass();
		void test();
		~DeClass();
};

/*
* 
*/
void TestPureClass ();

#endif