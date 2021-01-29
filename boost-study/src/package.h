#pragma once
#include "global_declare.h"

const ID_TYPE UT_Fid_BaseData = 10000;
class BaseData
{

public:
    virtual ~BaseData() 
    {
        cout << "~BaseData" << endl;
    }

    static const long Fid = UT_Fid_BaseData; 
};

DECLARE_PTR(BaseData);

const ID_TYPE UT_Fid_TestData = 10001;
class TestData:public BaseData
{

public:
    TestData(std::string name, std::string time):name_(name), time_(time)
    {
        cout << "TestData: " << name_ << " " << time_ << endl;
    }

    virtual ~TestData() 
    {
        cout << "~TestData" << endl;
    }

    static const long Fid = UT_Fid_TestData; 

    string get_name () { return name_;}
    string get_time () { return time_;}

private:
    std::string         name_;
    std::string         time_;
};

DECLARE_PTR(TestData);

class Package
{
public:
    std::map<ID_TYPE, BaseDataPtr>  data_map_;
};

DECLARE_PTR(Package);


template<class UserClass, typename... Args>
boost::shared_ptr<UserClass> CreateField(PackagePtr package, const Args&... args)
{
    try
    {
        boost::shared_ptr<UserClass> result = nullptr;

        if (package->data_map_.find(UserClass::Fid) != package->data_map_.end())
        {
            cout << "\nRellocate Data: " << UserClass::Fid << endl;
            UserClass* data = new(package->data_map_[UserClass::Fid].get()) UserClass(args...);

            result = boost::dynamic_pointer_cast<UserClass>(package->data_map_[UserClass::Fid]);
        }
        else 
        {
            cout << "\nCreate New Data: " << UserClass::Fid << endl;
            result = boost::make_shared<UserClass>(args...);
            package->data_map_[UserClass::Fid] = boost::dynamic_pointer_cast<BaseData>(result);
        }
        
        return result;
    }
    catch(const std::exception& e)
    {
        std::stringstream stream_obj;
        stream_obj << "[E] create_field " << e.what() << "\n";
        cout << "";
    }        
    catch(...)
    {
        cout << "[E] create_field Unknown Exceptions! " << endl;
    }
}

template<class UserClass>
boost::shared_ptr<UserClass> GetField(PackagePtr package)
{
    try
    {
        boost::shared_ptr<UserClass> result = nullptr;

        if (package->data_map_.find(UserClass::Fid) != package->data_map_.end())
        {
            cout << "\nGet Data: " << UserClass::Fid << endl;

            result = boost::dynamic_pointer_cast<UserClass>(package->data_map_[UserClass::Fid]);
        }

        return result;
    }
    catch(const std::exception& e)
    {
        std::stringstream stream_obj;
        stream_obj << "[E] GetField " << e.what() << "\n";
        cout << stream_obj.str() << endl;
    }        
    catch(...)
    {
        cout << "[E] GetField Unknown Exceptions! " << endl;
    }    
}

void TestPackageMain();