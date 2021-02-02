#pragma once
#include "global_declare.h"
#include "pandora/package/package_base.h"

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


class Package:public PackageBase
{
public:
    std::map<ID_TYPE, BaseDataPtr>  data_map_;

    void reset_fields() { }
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

template<class UserClass, typename... Args>
PackagePtr CreatePackage(const Args&... args)
{
    PackagePtr package = boost::make_shared<Package>();

    if (!CreateField<UserClass>(package, args...))
    {
        package = nullptr;
    }
    return package;
}


void TestPackageMain();