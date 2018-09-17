#include "test_static.h"

int TestStaticClassFunc::m_value = 10;

TestStaticClassFunc::TestStaticClassFunc() {
    m_value = 10;
}

TestStaticClassFunc& TestStaticClassFunc::Instance() {
    static TestStaticClassFunc TestStaticClassFuncObj;
    return TestStaticClassFuncObj;
}

void TestStaticClassFunc::addValue(int addedValue) {
    m_value += addedValue;
}

int TestStaticClassFunc::getValue() {
    return m_value;
}