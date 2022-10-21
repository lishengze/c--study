#include "moka_matcher.h"

TEST(TestMockPrivate, TestPrivate) {

    MockFoo foo;

    //GetTimeOut是private修饰的

    EXPECT_CALL(foo, GetTimeOut()).WillOnce(Return(1));

    cout << "test private GetTimeOut:" << foo.GetTimeOut() << endl; //1
}

// //测试重载方法

TEST(TestMockOverload, TestOverload) {
    MockFoo foo;
    EXPECT_CALL(foo, Add(_))
        .Times(1)
        .WillOnce(Return(1));
    cout << "test TestOverload Add:" << foo.Add('c') << endl; //1
}

//测试数量相同，类型不同的情况。

// virtual int Add(int x,int y) = 0;

// virtual int Add(int times, char x) = 0;

TEST(TestMockOverload, TestSameNumArg) {

    MockFoo foo;

    //两个都是int

    EXPECT_CALL(foo, Add(An<int>(), An<int>()))
        .Times(1)
        .WillOnce(Return(8));

    int c = foo.Add(3, 5);

    cout << "test TestOverload Add:" <<c<< endl; //8

     

    EXPECT_CALL(foo, Add(Matcher<int>(Lt(10)), TypedEq<char>('c')))
        .Times(1)
        .WillOnce(Return(7));
    c = foo.Add(2, 'c');
    cout << "test TestOverload Add:" << c << endl; //7
}

//测试数量相同，类型不同的情况。

TEST(TestMockOverload, TestSameNumArg2) {

   MockFoo foo;

   EXPECT_CALL(foo, Add(Matcher<int>(Lt(5)), An<int>()))
        .Times(1)
        .WillOnce(Return(7));

    int c = foo.Add(2, 5);

    cout << "test TestOverload Add:" << c << endl; //7

    //第一个参数小于5，,第二个参数是'd'

    EXPECT_CALL(foo, Add(Matcher<int>(Lt(5)), Matcher<char>('d')))
        .Times(1)
        .WillOnce(Return(10));

    c = foo.Add(2, 'd'); //10

    cout << "test TestOverload Add:" << c << endl; //10
}