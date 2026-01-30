#include "test_pointer.h"
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class VisitorCallable
{
public:
    typedef bool (VisitorCallable::*EntryType)(void*);
};

typedef bool (*SingleEntryType)(void*);

class  Visitor {
    public:
        Visitor();
        ~Visitor();
        bool RegisterClassMemberFunctionEntry(VisitorCallable::EntryType entry) {

            return true;
        }

        bool RegisterClassMemberFunctionEntry(SingleEntryType entry) {
            
            return true;
        }
};


struct MessageBase {
    MessageBase() {
        message_ = "MessageBase";
    }
    string message_;
};

struct Message1 {
    Message1() {
        message_ = "Message1";
    }
    string message_;
};

struct Message2 {
    Message2() {
        message_ = "Message2";
    }
    string message_;
};



class TestPointer1 {

    public:
        TestPointer1() {
            sClassName_ = "TestPointer1";
        }

        bool PrintPointer1(MessageBase* pMsg) {
            // cout << "PrintPointer1: " << pMsg->message_ << endl;

            cout << "PrintPointer1: "  << sClassName_ << endl;
            return true;
        }

        string sClassName_;
};

class TestPointer2 {
    public:
        TestPointer2() {
            sClassName_ = "TestPointer2";
        }

        bool PrintPointer2(MessageBase* pMsg) {
            // cout << "PrintPointer2: " << pMsg->message_ << endl;

            cout << "PrintPointer2: " << sClassName_ << endl;
            return true;
        }

        string sClassName_;
};

void calc_tmp(int i) {
    int a = i & 7;
    int b = a >> 3;

    cout << "i: " << i << ", a: " << a << ", b: " << b << endl;
}

void TestOperator() {
    std::vector<int> v = {0, 8, 16, 24, 1, 9, 17, 25, 2, 10, 18, 26};
    for (int i : v) {
        calc_tmp(i);
    }
}

void TestPointer() {
    // TestPointer1 t1;
    // TestPointer2 t2;
    // Message1 msg1;
    // Message2 msg2;

    // SingleEntryType pFuncSingleEntryType1 = (SingleEntryType)(&TestPointer1::PrintPointer1);
    // SingleEntryType pFuncSingleEntryType2 = (SingleEntryType)(&TestPointer2::PrintPointer2);

    // pFuncSingleEntryType1(&msg1);
    // pFuncSingleEntryType2(&msg2);

    // VisitorCallable::EntryType pFuncEntryType1 = (VisitorCallable::EntryType )&TestPointer1::PrintPointer1;
    // VisitorCallable::EntryType pFuncEntryType2 = (VisitorCallable::EntryType )&TestPointer2::PrintPointer2;

    // ( ( (VisitorCallable*)(&t1) ) ->*pFuncEntryType1 )(&msg1);


    TestOperator();

}

