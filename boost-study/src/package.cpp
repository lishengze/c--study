#include "package.h"
#include "global_declare.h"

#include "pandora/util/time_util.h"

void test_basic_packge()
{
    PackagePtr packge = boost::make_shared<Package>();
    BaseDataPtr base_ptr = CreateField<BaseData>(packge);

    if (base_ptr)
    {
        cout << "Create BaseData Success!" << endl;
    }
    else 
    {
        cout << "Create BaseData Failed!" << endl;
    }
}

void test_real_packge()
{
    PackagePtr packge = boost::make_shared<Package>();

    std::string name = "Create New";
    std::string time = utrade::pandora::NanoTimeStr();

    TestDataPtr data_ptr = CreateField<TestData>(packge, name, time);

    if (data_ptr)
    {
        cout << "Create TestData Success!" << endl;
    }
    else 
    {
        cout << "Create TestData Failed!" << endl;
    }

    name = "Reallocate New";
    time = utrade::pandora::NanoTimeStr();

    data_ptr = CreateField<TestData>(packge, name, time);

    if (data_ptr)
    {
        cout << "Reallocate TestData Success!" << endl;
    }
    else 
    {
        cout << "Reallocate TestData Failed!" << endl;   
    } 

    data_ptr = GetField<TestData>(packge);
    if (data_ptr)
    {
        cout << "GetField TestData Success! " << data_ptr->get_name() << " " << data_ptr->get_time() << endl;
    }
    else 
    {
        cout << "GetField TestData Failed!" << endl;   
    }     

}

void TestPackageMain()
{
    cout << "TestPackageMain " << endl;

    // test_basic_packge();

    test_real_packge();
}