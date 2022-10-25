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
int CalPrimePalindromeFactorialALL(int n) {
  // �ж�����
    if (n <= 1) return 0;
    if (n % 2 == 0 && n !=2 ) return 0;

    for (int i = 3;; i += 2) {
        if (i > n / i) break;
        if (n % i == 0) return 0;
    }

  // �жϻ�����
    if(n<0) return 0;
    double rn=0;
    int nn=n;
    while(n)
    {
        rn=rn*10+n%10;
        n/=10;
    }
    if (rn!=nn) return 0;

    // ����׳�
    int result = 1;
    for (int i = 1; i <= n; i++) {
        result *= i;
    }
    return result;
}

int CalPrimePalindromeFactorial(int n) {
    if (!IsPrime(n)) return 0;

    if(!IsPalindrome(n)) return 0;

    return Factorial(n);
}

TEST(TEST_ALL, TestPrime) {
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(1));
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(2));
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(3));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(4));
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(11));
    EXPECT_TRUE(CalPrimePalindromeFactorialALL(17));
    EXPECT_FALSE(CalPrimePalindromeFactorialALL(21));
}

TEST(TEST_ALL, TestPalindrome) {
    
}

TEST(TEST_ALL, TestFactorial) {
    
}