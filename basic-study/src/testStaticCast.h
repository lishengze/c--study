#ifndef TESTSTATICCAST_H
#define TESTSTATICCAST_H

class base1 {
	public:
	virtual void func();
};

class derived1:public base1 {
	public:
	void func();

	void funcd();
};

class derived2:public derived1 {
	public:
	void func();

	void funcd();
};

class derived3:public base1 {
	public:
	void func();

	void funcd();
};

void testStaticCast();

#endif