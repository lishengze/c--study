#include "global_declare.h"

// ����׳�
int Factorial(int n) {
  int result = 1;
  for (int i = 1; i <= n; i++) {
    result *= i;
  }
  return result;
}

// �Ƿ�������
bool IsPrime(int n) {
  if (n <= 1) return false;
  if (n % 2 == 0) return n == 2;

  for (int i = 3;; i += 2) {
    if (i > n / i) break;

    if (n % i == 0) return false;
  }
  return true;
}

// �Ƿ��ǻ�����
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

// �����ǻ������������Ľ׳�;
int CalPrimePalindromeFactorialSeprate(int n) {
    if (!IsPrime(n)) return 0;
    if(!IsPalindrome(n)) return 0;
    return Factorial(n);
}

// �����ǻ������������Ľ׳�;
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


// Ϊ����ʽʵ�ֵĽӿڱ�д�����׼�;
// Ϊÿ����Զ����Ĺ��ܵ��д�����Ĳ��԰���;
// ���ǲ��ԵĽӿ���ͬһ��;
TEST(TEST_ALL, TestPrime) {
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(2));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(4));
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(11));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(21));
}

TEST(TEST_ALL, TestPalindrome) {
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(2));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(4));
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(121));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(221));   
}

TEST(TEST_ALL, TestFactorial) {
	EXPECT_EQ(1, CalPrimePalindromeFactorialALL(1));
	EXPECT_EQ(2, CalPrimePalindromeFactorialALL(2));
	EXPECT_EQ(3, CalPrimePalindromeFactorialALL(3));
	EXPECT_EQ(120, CalPrimePalindromeFactorialALL(5));    
}

// Ϊ��������ʽʵ�ֵĽӿڱ�д�����׼�;
// Ϊÿ�����Խӿڱ�д�����Ĳ��԰�����ÿ�β��ԵĽӿڲ�ͬ;
TEST(TEST_SEPARTE, TestPrime) {
    EXPECT_TRUE(IsPrime(2));
    EXPECT_FALSE(IsPrime(4));
    EXPECT_TRUE(IsPrime(11));
    EXPECT_FALSE(IsPrime(21));
}

TEST(TEST_SEPARTE, TestPalindrome) {
    EXPECT_TRUE(IsPalindrome(2));
    EXPECT_FALSE(IsPalindrome(4));
    EXPECT_TRUE(IsPalindrome(121));
    EXPECT_FALSE(IsPalindrome(221));    
}

TEST(TEST_SEPARTE, TestFactorial) {
	EXPECT_EQ(1, Factorial(1));
	EXPECT_EQ(2, Factorial(2));
	EXPECT_EQ(3, Factorial(3));
	EXPECT_EQ(120, Factorial(5));    
}

int FuncCall1(int n) {
    // Before Work;
    ++n;
    // After Work;
    return n;
}

int FuncCall2(int n) {
    // Before Work;
    ++n;
    ++n;
    ++n;
    ++n;
    // After Work;
    return n;
}

void FuncCall3(int n) {
    // Before Work;
    ++n;
    --n;
    ++n;
    --n;
    n ^= 1;
    --n;
    ++n;
    ++n;
    // After Work;
}

void FuncCall4(int n) {
    // Before Work;
    ++n;
    --n;
    n ^= n++;
    ++n;
    --n;
    n |= n--;
    --n;
    ++n;
    ++n;
    // After Work;
}