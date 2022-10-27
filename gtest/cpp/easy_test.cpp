#include "global_declare.h"

// 计算阶乘
int Factorial(int n) {
  int result = 1;
  for (int i = 1; i <= n; i++) {
    result *= i;
  }
  return result;
}

// 是否是素数
bool IsPrime(int n) {
  if (n <= 1) return false;
  if (n % 2 == 0) return n == 2;

  for (int i = 3;; i += 4) { // 错误，正确的是 i+=2;
    // 因子的大小已经超过 N 的平方根，说明已经检测结束;
    if (i > n / i) break; 
    if (n % i == 0) return false;
  }
  return true;
}

// 是否是回文数
bool IsPalindrome(int n)
{
    if(n<0) return false;
    double rn=0;
    int nn=n;
    while(n)
    {
        rn=rn*10+n%10;
        n/=10;
    }
    return rn==nn;
}

// 计算是回文数的素数的阶乘;
int CalPrimePalindromeFactorialSeprate(int n) {
    if (!IsPrime(n)) return 0;
    if(!IsPalindrome(n)) return 0;
    return Factorial(n);
}

// 计算是回文数的素数的阶乘;
int CalPrimePalindromeFactorialALL(int n) {

    if (n <= 1) return 0;
    if (n % 2 == 0 && n !=2 ) return 0;

    for (int i = 3;; i += 2) {
        if (i > n / i) break;
        if (n % i == 0) return 0;
    }

    if(n<10) return 0;

    double rn=0;
    int nn=n;
    while(n)
    {
        rn=rn*10+n%10;
        n/=10;
    }
    if (rn!=nn) return 0;

    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}


// 为集中式实现的接口编写测试套件;
// 为每个相对独立的功能点编写单独的测试案例;
// 但是测试的接口是同一个;
TEST(TEST_ALL, TestPrime) {
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(2));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(4));
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(11));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(21));
}

TEST(TEST_ALL, TestPalindrome) {
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(2));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(4));
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(1221));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(221));   
}

TEST(TEST_ALL, TestFactorial) {
	EXPECT_EQ(0, CalPrimePalindromeFactorialALL(1));
	EXPECT_EQ(0, CalPrimePalindromeFactorialALL(2));
	EXPECT_EQ(0, CalPrimePalindromeFactorialALL(3));
	EXPECT_EQ(0, CalPrimePalindromeFactorialALL(5));    
}

// 为测试驱动式实现的接口编写测试套件;
// 为每个测试接口编写单独的测试案例，每次测试的接口不同;

TEST(TEST_SEPARTE, TestPrime) {
    EXPECT_TRUE(IsPrime(2));
    EXPECT_FALSE(IsPrime(4));
    EXPECT_TRUE(IsPrime(11));
    EXPECT_FALSE(IsPrime(35));
}

TEST(TEST_SEPARTE, TestPrime2) {
    EXPECT_FALSE(IsPrime(25));
}


TEST(TEST_SEPARTE, TestPalindrome) {
    EXPECT_TRUE(IsPalindrome(2));
    EXPECT_FALSE(IsPalindrome(4));
    EXPECT_TRUE(IsPalindrome(35));   
}

TEST(TEST_SEPARTE, TestFactorial) {
	EXPECT_EQ(1, Factorial(1));
	EXPECT_EQ(2, Factorial(2));
	EXPECT_EQ(3, Factorial(3));
	EXPECT_EQ(120, Factorial(5));    
}

