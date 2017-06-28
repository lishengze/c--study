#include "testPureClass.h"
#include "print.h"

PureClass::PureClass() {
	cout << "PureClass" << endl;
	// m_mutuble = 0;
}

PureClass::~PureClass() {
	cout << "~PureClass" << endl;
}


DeClass::DeClass(){
	cout << "DeClass" << endl;
}

void DeClass::test() {
	cout << "test" << endl;
}

DeClass::~DeClass() {
	cout << "~DeClass" << endl;
}

void TestPureClass () {
	DeClass tmpObj;
	// cout <<tmpObj.m_mutuble << endl;

	// PureClass* pcPtr = &tmpObj;
	// delete pcPtr;
}