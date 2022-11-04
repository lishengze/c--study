#include <iostream>
using namespace std;
 
class API {    //旧的API
    public:
        void old_apply() {
            cout<<"old api"<<endl;
        }
};
 
class new_API {  //新的API
    public:
        void new_apply() {
            cout<<"new api"<<endl;
        }
};
 
class dep_API :public API{  //标记为要废弃的API
    public:
        new_API *_api;
        dep_API() {
            _api = new new_API();
        }
        void new_apply() {
            _api->new_apply();
        }
};
 
void apply(int a,int b,int c) {
    //API a1;  原先的老的接口函数
    //a1.old_apply();
    dep_API a1;
    a1.new_apply(); 
}
 
void apply(int a,int b) {
    new_API a1;
    a1.new_apply();
}
 
int TestAdapterObjectExtend() {
    apply(1,2,3);
    apply(1,2);
    return 0;
}