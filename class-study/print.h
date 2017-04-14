#ifndef _PRINT_H_
#define _PRINT_H_

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <string>
using std::string;

template<class datatype>
void PrintData(string dataDescription, datatype* data, int datalength) {
	cout << dataDescription << endl;
	for (int i = 0; i < datalength; ++i) {
		cout << *(data++) << " ";
	}
	cout << endl;
}

template<class datatype>
void PrintData(string dataDescription, datatype data) {
	cout << dataDescription << endl;
	cout << data << endl;
}

#endif // !_PRINT_H_
