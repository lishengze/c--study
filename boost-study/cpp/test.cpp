#include "test.h"

// #include "SimplePackage.h"

#include "test_shared_ptr.h"
#include "global_declare.h"

// #include "double.h"


void test_boost_1_73() {
	std::string basePath;
	if (basePath.empty())
	{
		basePath = boost::filesystem::initial_path<boost::filesystem::path>().string();
	}    
}

void testFuncMain()
{
    test_shared_ptr();
    test_boost_1_73();

    // TestPackageMain();

    // TestNumbMain();
}
