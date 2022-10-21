#pragma once
#include "global_declare.h"



struct SimpleDataType{ 
  int value1_;
  float value2_;

  void Init() {
    value1_ = 1;
    value2_ = 2;
  }
};

class SimpleAlgorithm {
    public:
        virtual bool IsPositive(int value) { 
            return value > 0;
        }

        virtual ~SimpleAlgorithm() {}
};

class UserAlogrithm {
  public:
    UserAlogrithm() {
        private_value_.Init();
        public_value_.Init();
    }

    void SetSimpleAlgo(SimpleAlgorithm* simple_algo) {
        simple_algo_ = simple_algo;
    }

    int Factorial(int n) {
      int result = 1;
      for (int i = 1; i <= n; i++) {
        result *= i;
      }
      return result;
    }



    bool IsPrime(int n) {
      if (n <= 1) return false;
      if (n % 2 == 0) return n == 2;
      for (int i = 3;; i += 2) {
        if (i > n / i) break;
        if (n % i == 0) return false;
      }
      return true;
    }

    int StringCmp(const char* a, const char* b) {
        return strcmp(a, b);
    }

    char* StringCat(char* src, char* dest) {
      strcat(dest, src);
      cout << "src: " << src << ", dest: " << dest << endl;
      return dest;
    }

    bool IsStringEqual(const char* a, const char* b) {
      return StringCmp(a, b) == 0;
    }

    int FactorialCombine(int n) {
        if (!simple_algo_->IsPositive(n)) return 0;
        int result = 1;
        for (int i = 1; i <= n; i++) {
            result *= i;
        }
        return result;
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


  public:
    SimpleDataType public_value_;
    SimpleAlgorithm* simple_algo_;

  private:
    SimpleDataType private_value_;
};
