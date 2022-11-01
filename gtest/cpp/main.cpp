//============================================================================
// Name        : test google test
// Author      : lsz
// Version     :
// Copyright   : None
// Description : Study Google Test
// Date        : 2022-9-22
//============================================================================
#include "global_declare.h"
#include "base.h"
#include "gtest/gtest.h"
#include <stdio.h>
#include "moka_base.h"

class FooEnvironment : public testing::Environment
{
public:
    virtual void SetUp()
    {
        std::cout << "Foo FooEnvironment SetUP" << std::endl;
		env_ = "This is FooEnvironment";
    }
    virtual void TearDown()
    {
        std::cout << "Foo FooEnvironment TearDown" << std::endl;
    }

	string env_;
};

int main(int argc, char* argv[]) 
{
	printf("---------- Test GoogleTest ----------\n");

    testing::GTEST_FLAG(filter) = "AlgoTest*";
    testing::GTEST_FLAG(repeat) = 1;
    testing::GTEST_FLAG(output) = "xml:";

	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
