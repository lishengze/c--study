//============================================================================
// Name        : test google test
// Author      : lsz
// Version     :
// Copyright   : None
// Description : Study right refrenece in C++, Ansi-style
// Date        : 2022-8-16
//============================================================================
#include "global_declare.h"
#include "base.h"
#include "gtest/gtest.h"
#include "gtest/gtest-assertion-result.h"
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

    testing::GTEST_FLAG(filter) = "Algo*:Share*";
    testing::GTEST_FLAG(repeat) = 1;
    testing::GTEST_FLAG(output) = "xml:";

	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
