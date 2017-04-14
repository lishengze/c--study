#include "testVirtualBase.h"

void TestVirtualBase() {
	Base baseObj;
	DerivedA derivedAObj;
	DerivedB derivedBObj;
	DerivedC derivedCObj;

	baseObj.m_value = 10;
	derivedAObj.m_value = 14;
	derivedBObj.m_value = 12;

	PrintData("baseObj.m_value: ", baseObj.m_value);
	PrintData("derivedAObj.m_value: ", derivedAObj.m_value);
	PrintData("derivedBObj.m_value: ", derivedBObj.m_value);
	PrintData("derivedCObj.m_value: ", derivedCObj.m_value);
}