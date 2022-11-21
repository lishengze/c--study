#include "test.h"




// void test_member_obj(int count) {
//     for (int i=0; i<count; ++i) {
//         g_member_obj.base_obj_.SetA(i);
//     }
// }

// void test_inherit_class(int count) {
    
//     for (int i=0; i<count; ++i) {
//         g_inherit_obj.SetA(i);
//     }    
// }   


void TestMember(benchmark::State& state) {
    MemberClass member_obj;
     for (auto _: state) {

        member_obj.base_obj_.UpdateA();
     }
    
}

void TestInherit(benchmark::State& state) {
    InheritClass inherit_obj;
    for (auto _: state) {         
        inherit_obj.UpdateA();
    }
}

void TestMemberPointer(benchmark::State& state) {
    MemberClass* member_obj = new MemberClass();

    if (member_obj) {
        for (auto _: state) {
            member_obj->base_obj_.UpdateA();
        }

        delete member_obj;
    }
}

void TestInheritPointer(benchmark::State& state) {
    InheritClass* inherit_obj = new InheritClass();

    if (inherit_obj) {
        for (auto _: state) {         
            inherit_obj->UpdateA();
        }
        delete inherit_obj;
    }
}