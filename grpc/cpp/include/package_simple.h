#pragma once
#include "global_declare.h"
#include "package_base.h"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

const long long UT_Fid_BaseData = 10000;
class PacakgeBaseData
{

public:
    virtual ~PacakgeBaseData() 
    {
        // cout << "~PacakgeBaseData" << endl;
    }

    static const long Fid = UT_Fid_BaseData; 
};
DECLARE_PTR(PacakgeBaseData);


class Package:public PackageBase
{
public:
    std::map<long long, PacakgeBaseDataPtr>  data_map_;

    void reset_fields() { }
};
DECLARE_PTR(Package);


template<class UserClass, typename... Args>
boost::shared_ptr<UserClass> CreateField(PackagePtr package, Args&&... args)
{
    try
    {
        boost::shared_ptr<UserClass> result = nullptr;

        if (package->data_map_.find(UserClass::Fid) != package->data_map_.end())
        {
            // cout << "\npackage_simple Rellocate Data: " << UserClass::Fid << endl;
            // UserClass* data = new(package->data_map_[UserClass::Fid].get()) UserClass(args...);

            UserClass* data = new(package->data_map_[UserClass::Fid].get()) UserClass(std::forward<Args>(args)...);

            result = boost::dynamic_pointer_cast<UserClass>(package->data_map_[UserClass::Fid]);
        }
        else 
        {
            // cout << "\npackage_simple Create New Data: " << UserClass::Fid << endl;

            // result = boost::make_shared<UserClass>(args...);

            result = boost::make_shared<UserClass>(std::forward<Args>(args)...);

            package->data_map_[UserClass::Fid] = boost::dynamic_pointer_cast<PacakgeBaseData>(result);
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
            // cout << "\nGet Data: " << UserClass::Fid << endl;

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
PackagePtr CreatePackage(Args&&... args)
{
    PackagePtr package = boost::make_shared<Package>();

    if (!CreateField<UserClass>(package, std::forward<Args>(args)...))
    {
        package = nullptr;
    }

    // if (!CreateField<UserClass>(package, args...))
    // {
    //     package = nullptr;
    // }    
    return package;
}

class IPackageStation;
class IPackageStation
{
public:
    IPackageStation(IPackageStation* next_station) : next_station_{next_station}
    {
        if (next_station_)  next_station_->set_fore_station(this);
    }
    virtual ~IPackageStation() {}
    // 启动站点运营
    virtual void launch() {}
    // 释放运营站点
    virtual void release() {}
    // 设定下一个站点
    void set_fore_station(IPackageStation* fore_station) { fore_station_=fore_station; }
    // 所有的投递站点都需要继承这两个接口
    virtual void request_message(PackagePtr package) {}
    virtual void response_message(PackagePtr package) {}
    
    // 投递请求数据包到下一站点
    void deliver_request(PackagePtr package) { if(next_station_ && package) next_station_->request_message(package); }
    // 投递回应数据包到下一站点
    void deliver_response(PackagePtr package) {if(fore_station_ && package) fore_station_->response_message(package); }
protected:
    IPackageStation* fore_station_{nullptr};   // 上一站点，数据进来站点为初始站
    IPackageStation* next_station_{nullptr};   // 下一站点，数据出去站点为终点站
};
DECLARE_PTR(IPackageStation);