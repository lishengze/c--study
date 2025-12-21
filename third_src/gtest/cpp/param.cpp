#include "global_declare.h"

struct Tuples
{
    int a;
    int b;
    int c;
};
 
class FuncTestParamStruct:public testing::TestWithParam<struct Tuples>
{
protected:
    void SetUp() override {
        t1.a = GetParam().a;
        t1.b = GetParam().b;
        t1.c = GetParam().c;
    }
    int add(int x,int y,int z){
        return x+y+z;
    }
    Tuples t1;
};
 
TEST_P(FuncTestParamStruct,case1)
{
    // EXPECT_EQ(t1.a+t1.b+t1.c,add(t1.a,t1.b,t1.c));
    auto data = GetParam();
    EXPECT_EQ(data.a+data.b+data.c,add(data.a,data.b,data.c));
}
 
INSTANTIATE_TEST_CASE_P(StructTest, FuncTestParamStruct, 
                        testing::Values(Tuples{1,2,3},Tuples{4,5,6}));
 
class FuncTestParamCompbine:public testing::TestWithParam<tuple<int,int>>{
protected:
    void SetUp() override
    {
        a = get<0>(GetParam());
        b = get<1>(GetParam());
    }
    int a;
    int b;
};
 
int func(int a,int b)
{
    return a+b;
}
 
// TEST_P(FuncTestParamCompbine, case1)
// {
//     cout<<a<<"+"<<b<<"="<<func(a,b)<<endl;
// }
 
 
// INSTANTIATE_TEST_CASE_P(
//         CombineTest,
//         FuncTestParamCompbine,
//         testing::Combine(testing::Values(1,3,4),testing::Values(2,3,4)));
