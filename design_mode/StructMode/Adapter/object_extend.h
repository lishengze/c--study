#include <iostream>
using namespace std;
 
class API {    //�ɵ�API
    public:
        void old_apply() {
            cout<<"old api"<<endl;
        }
};
 
class new_API {  //�µ�API
    public:
        void new_apply() {
            cout<<"new api"<<endl;
        }
};
 
class dep_API :public API{  //���ΪҪ������API
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
    //API a1;  ԭ�ȵ��ϵĽӿں���
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