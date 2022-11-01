#include "base.h"
#include "global_declare.h"
#include "gtest/gtest.h"
#include <string.h>

class MockSimpleAlgorithm: public SimpleAlgorithm {
  public:
    MOCK_METHOD1(IsPositive, bool(int));

    virtual ~MockSimpleAlgorithm() {}
};

class TestUserAlogrithm: public UserAlogrithm {
  public:
    bool SimpleDataEqual(const SimpleDataType& src, const SimpleDataType& target) { 
      return src.value1_ == target.value1_ && src.value2_ == target.value2_;
    }

    bool CheckPrivateData(const SimpleDataType& target) { 
      return SimpleDataEqual(private_value_, target);
    }

    bool CheckPublicData(const SimpleDataType& target) { 
      return SimpleDataEqual(public_value_, target);
    }
};


TEST(AlgoTest,AlgoInt) {
  UserAlogrithm algoObj;

	EXPECT_EQ(1, algoObj.Factorial(1));
	EXPECT_EQ(2, algoObj.Factorial(2));
	EXPECT_EQ(3, algoObj.Factorial(3));
	EXPECT_EQ(40320, algoObj.Factorial(8));

  // EXPECT_EQ(10, algoObj.public_value_);

	// EXPECT_TRUE(algoObj.IsFactorialTrue(3,3));
	// EXPECT_PRED_FORMAT2(algoObj.IsFactorialTrueFormat, 3,3);
}

TEST(AlgoTest, AlgoString) {
  UserAlogrithm algoObj;
  char a[20] = "Hello";
  char b[20] = "World";
  EXPECT_EQ(0, algoObj.StringCmp(algoObj.StringCat(a, b), "Hello World"));


}

TEST(AlgoTest, AlgoPrivate) {
  TestUserAlogrithm algoObj;
  SimpleDataType target;
  target.Init();
  EXPECT_TRUE(algoObj.CheckPrivateData(target));
}

TEST(AlgoTest, AlgoPublic) {
  TestUserAlogrithm algoObj;
  SimpleDataType target;
  target.Init();
  EXPECT_TRUE(algoObj.CheckPublicData(target));
}


TEST(AlgoTest, AlgoMock) {
  UserAlogrithm algoObj;
  MockSimpleAlgorithm* mock_algo_ = new MockSimpleAlgorithm();
  algoObj.SetSimpleAlgo(mock_algo_);

  ON_CALL(*mock_algo_, IsPositive(_)).WillByDefault(Return(true));
  EXPECT_CALL(*mock_algo_, IsPositive(Lt(10))).WillRepeatedly(Return(false));

  EXPECT_EQ(0, algoObj.FactorialCombine(1));
  EXPECT_EQ(1, algoObj.FactorialCombine(1));

  if (mock_algo_) delete mock_algo_;
}

class ShareTest:public testing::Test {
  public:
    static TestUserAlogrithm algoObj_;
};

TestUserAlogrithm ShareTest::algoObj_;

TEST_F(ShareTest, AlgoPrivate) {
  SimpleDataType target;
  target.Init();
  EXPECT_TRUE(algoObj_.CheckPrivateData(target));
}

TEST_F(ShareTest, AlgoPublic) {
  SimpleDataType target;
  target.Init();
  EXPECT_TRUE(algoObj_.CheckPublicData(target));
}
