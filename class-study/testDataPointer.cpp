#include "testDataPointer.h"
#include "print.h"

void Point3d::test() {
	cout << "Pointer3d::test" << endl;
}

void testDataPointerFunc() {
	printf("&Point3d::x = %p \n", &Point3d::m_x);
	printf("&Point3d::y = %p \n", &Point3d::m_y);
	printf("&Point3d::z = %p \n", &Point3d::m_z);
}