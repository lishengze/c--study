#include "testOverloaded.h"
#include "print.h"

// void func1(int val) {
// 	cout << "func1" << endl;
// 	PrintData("func1: ", val);
// }

// void func1(const int val){
// 	PrintData("func1-const: ", val);
// }

void func2(int& val) {
	PrintData("func2: ", val);
}

void func2(const int& val){
	PrintData("func2-const: ", val);
}