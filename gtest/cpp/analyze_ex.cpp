#include "global_declare.h"

class TestCase {
    public:

    TestCase() {}

    int Func1(int n1) {
        int local_n2 = 10;
        n1 = n1 + local_n2;
        if (n1 > 100) {
            m_value1++;
            m_value2++;
        }
        return n1;
    }

    private:
        int m_value1;
        int m_value2;
        int m_value3;

    public:
    int Func2(int value) {
        int local_n = 100;
        value += Cal1(local_n);
        m_value3 += value;
        return value;
    }

    int Cal1(int n2) {
        return n2*2;
    }

    int Func3(int value) {
        m_value1 += value;
        if (m_value1 > 100) {
            // 此次接口的调用是可测的,m_value1 在逻辑判断后和外部接口调用后一直未变，在接口调用结束后可以在外部校验;
            PrintImportantInfo(m_value1); 
        }

        int condition1 = m_value1 + m_value2;
        if (condition1 < 0) {
            // 此次接口的调用不可测，condition1 是局部变量，即时它在此次调用后不改变，也无法被外部获取;
            PrintImportantInfo(condition1); 
            return condition1;
        }

        m_value2 += condition1;
        if (m_value2 > 10) {
            // 此次接口的调用不可测，m_value2 是非局部变量，可以被外部获取，
            // 但是它在此次调用后变化了, 这个变化的过程不一定是可逆的，Fun3() 执行完后，再校验m_value2 的值没有意义;
            PrintImportantInfo(m_value2);
        }
        m_value2 /= (m_value2 + m_value1 + 100); // 发生变化;

        m_value3 -= m_value2;
        m_value3 *= 99 + m_value2*m_value1 + value * 1331;
        if (m_value3 < -1) { // 重要的逻辑判断
            m_value3 =  (m_value2 / m_value1 + value % 100) / 7;
            // 此次接口调用相对复杂, 分为两层考虑
            // 接口的调用参数是可测的，m_value3 在此次调用后未改变，并且是对外可得的;
            // 接口的调用时机是不可测的, m_value3 在作为逻辑判断条件后，又经过了复杂的变化;
            //  在整个Func3()结束后,作为判断条件的m_value3 的值是不可复原的。
            // 所以此次外部接口的调用从整体来讲是不可测的。
            PrintImportantInfo(m_value3); 
        }
        return m_value3 - m_value2;
    }

    // 具有全局影响力的接口;
    void PrintImportantInfo(int value) {
        printf("%d", value);
    }
};
