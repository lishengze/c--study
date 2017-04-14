#include "data-sema.h"
#include "print.h"

void TestEmptyClassSize() {
	PrintData("A.size: ", sizeof(A));
	PrintData("B.size: ", sizeof(B));
	PrintData("C.size: ", sizeof(C));
	PrintData("D.size: ", sizeof(D));
}