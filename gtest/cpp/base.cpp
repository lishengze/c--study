#include "base.h"
#include "gtest/gtest.h"

int Factorial(int n) {
  int result = 1;
  for (int i = 1; i <= n; i++) {
    result *= i;
  }

  return result;
}

// Returns true if and only if n is a prime number.
bool IsPrime(int n) {
  // Trivial case 1: small numbers
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

testing::AssertionResult IsFactorialTrue(int a, int b) {
	if (Factorial(a) == b) {
		return testing::AssertionSuccess();
	} else {
		return testing::AssertionFailure() << "\n[HaHa]Factorial(" << a << ") !=" << b;
	}
}

testing::AssertionResult IsFactorialTrueFormat(const char* a_expr,const char* b_expr, int a, int b) {
	if (Factorial(a) == b) {
		return testing::AssertionSuccess();
	} else {
		return testing::AssertionFailure() << "\n[HaHa]Factorial(" << a << ") !=" << b 
				<< ", a_expr: " << a_expr << ", b_expr: " << b_expr << "\n";
	}
}

// TEST(FactorialTest, Positive) {
// 	EXPECT_EQ(1, Factorial(1));
// 	EXPECT_EQ(2, Factorial(2));
// 	EXPECT_EQ(3, Factorial(3));
// 	EXPECT_EQ(40320, Factorial(8));

// 	EXPECT_TRUE(IsFactorialTrue(3,3));
// 	EXPECT_PRED_FORMAT2(IsFactorialTrueFormat, 3,3);
// }