#include "moka_combine.h"

using namespace testing;

class BaseUser{
    public:
        virtual bool IsPositive(int value) { 
            return value > 0;
        }
};

class MockBaseUser: public BaseUser {
    public:
    MOCK_METHOD1(IsPositive, bool(int));
};

class TestWork{
    public:
        void SetUser(BaseUser* user) { 
            user_ = user;
        }

        bool TestPositive(int i) { 
            return user_->IsPositive(i);
        }
    BaseUser* user_;
};


class MockCombine:public testing::Test {
    public:
        void SetUp() override { 
            cout << __FILE__ << ", " << __FUNCTION__ << ": SetUp" << endl;

            user_ = new MockBaseUser();
            worker_.SetUser(user_);
        }

        void TearDown() override { 
            cout << __FILE__ << ", " << __FUNCTION__ << ": TearDown" << endl;

            if (user_) {
                delete user_;
            }
        }


        TestWork    worker_;
        MockBaseUser* user_;
};

TEST_F(MockCombine, BasicTest) {
    // EXPECT_CALL(*user_, IsPositive(Gt(10))).WillRepeatedly(testing::Return(true));
    EXPECT_CALL(*user_, IsPositive(Lt(10))).WillRepeatedly(testing::Return(false));

    EXPECT_TRUE(worker_.TestPositive(9));
    EXPECT_TRUE(worker_.TestPositive(11));
}