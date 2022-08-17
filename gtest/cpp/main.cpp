//============================================================================
// Name        : test google test
// Author      : lsz
// Version     :
// Copyright   : None
// Description : Study right refrenece in C++, Ansi-style
// Date        : 2022-8-16
//============================================================================

#include "base.h"
#include "gtest/gtest.h"
#include <stdio.h>

TEST(FactorialTest, Positive) {
EXPECT_EQ(1, Factorial(1));
EXPECT_EQ(2, Factorial(2));
EXPECT_EQ(3, Factorial(3));
EXPECT_EQ(40320, Factorial(8));
}

int main(int argc, char* argv[]) 
{
	printf("---------- Test GoogleTest ----------\n");

	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();

}
