#include "testOverloaded.h"

int main(int argc, char* argv[])
{
	func2(5);	
	int a = 6;
	const int& b = a;
	func2(b);

	return 0;
}

