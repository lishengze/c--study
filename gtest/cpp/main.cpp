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
#include "gtest/gtest-assertion-result.h"
#include <stdio.h>

int main(int argc, char* argv[]) 
{
	printf("---------- Test GoogleTest ----------\n");

	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();

}
