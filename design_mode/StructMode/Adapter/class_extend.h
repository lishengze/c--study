#include <iostream>
using namespace std;
enum sqlType {
    mysqlType = 1,
    sqlite3Type,
    postgersqlType
};
enum sqlType type;
class mysql {
    public:
        void select() {
            cout<<"mysql select"<<endl;
        }
};
 
class sqlite3 {
    public:
        void select() {
            cout<<"sqlite3 select"<<endl;
        }
};
 
class postgersql {
    public:
        void select() {
            cout<<"postgersql select"<<endl;
        }
};
 
class ORM:public mysql,public sqlite3,public postgersql {
    public:
        void orm_select() {
            switch(type) {
                case mysqlType:
                    mysql::select();
                    break;
                case sqlite3Type:
                    sqlite3::select();
                    break;
                case postgersqlType:
                    postgersql::select();
                    break;
                default:
                    break;
            }
        }
};
 
void init_sqltype(enum sqlType t){     //注册驱动是哪个类型
    type = t;
}
 
int TestAdapterClassExtend() {
    ORM *o = new ORM();
    init_sqltype(mysqlType);  
    o->orm_select();
    init_sqltype(sqlite3Type);
    o->orm_select();
    init_sqltype(postgersqlType);
    o->orm_select();
    delete o;
    return 0;
}