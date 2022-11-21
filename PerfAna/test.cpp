#include "test.h"

MemberClass g_member_obj;
InheritClass g_inherit_obj;

void test_member_obj(int count) {
    for (int i=0; i<count; ++i) {
        g_member_obj.base_obj_.SetA(i);
    }
}

void test_inherit_class(int count) {
    
    for (int i=0; i<count; ++i) {
        g_inherit_obj.SetA(i);
    }    
}   


void TestMemberObj(benchmark::State& state) {
    auto count = state.range(0);
     for (auto _: state) {

        // test_member_obj(count);
        // for (int i=0; i<count; ++i) {
        //     g_member_obj.base_obj_.SetA(i);
        // }

        g_member_obj.base_obj_.SetA(count);
     }
    
}

void TestInheritClass(benchmark::State& state) {
    auto count = state.range(0);

    for (auto _: state) {
        // test_inherit_class(count);
        // for (int i=0; i<count; ++i) {
        //     g_inherit_obj.SetA(i);
        // }          

        g_inherit_obj.SetA(count);
    }
}