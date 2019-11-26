#ifndef _TEST_CLASSSIZE_H_
#define _TEST_CLASSSIZE_H_

class Student1 {
	public:
		bool isGirl;
		bool isboy;
		bool age2;
		double age;
}; // size == 16;

class Student2 {
	public:
		bool isGirl;
		short age2;
		bool isboy;	
}; // size == 6;

class Student3: public Student2 {
	public: 
		bool is3;
}; // size == 8;

void TestClassSize ();

#endif