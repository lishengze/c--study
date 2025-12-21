#include "global_declare.h"

class TestObj {
    public:
        bool IsPrime(int n) {
            if (n <= 1) return false;

            // Trivial case 2: even numbers
            if (n % 2 == 0) return n == 2;

            // Now, we have that n is odd and n >= 3.

            // Try to divide n by every odd number i, starting from 3
            for (int i = 3;; i += 2) {
                // We only have to try i up to the square root of n
                if (i > n / i) break;

                // Now, we have i <= n/i < n.
                // If n is divisible by i, n is not prime.
                if (n % i == 0) return false;
            }

            // n has no integer factor in the range (1, n), and thus is prime.
            return true;            
        }

        int a;

};

class MyTest: public ::testing::Test {
public:
    void SetUp() override {
        obj.a = 100;
        printf("SetUp Set obj.a = %d\n", 100);
    }

    void TearDown() override {
        printf("TearDown");
    }

    TestObj obj;
};

// TEST_F(MyTest, TestF1) {
//     // cout << env_ << endl;

//     // testing::AddGlobalTestEnvironment
//     cout << "TestF1: a = " << obj.a << endl;
//     EXPECT_TRUE(obj.IsPrime(obj.a));
//     obj.a = 101;
// }

// TEST_F(MyTest, TestF2) {
//     cout << "TestF2: a = " << obj.a << endl;
//     obj.a = 77;
//     EXPECT_TRUE(obj.IsPrime(obj.a));
// }
