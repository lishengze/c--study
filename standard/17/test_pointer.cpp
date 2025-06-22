#include "test_pointer.h"
#include <string>
#include <iostream>
using namespace std;

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
        bool PrintPointer1(MessageBase* pMsg) {
            cout << "PrintPointer1: " << pMsg->message_ << endl;
            return true;
        }
};

class TestPointer2 {
    public:
        bool PrintPointer2(MessageBase* pMsg) {
            cout << "PrintPointer2: " << pMsg->message_ << endl;
            return true;
        }
};

void TestPointer() {
    TestPointer1 t1;
    TestPointer2 t2;
    Message1 msg1;
    Message2 msg2;

    SingleEntryType pFuncSingleEntryType1 = (SingleEntryType)(&TestPointer1::PrintPointer1);
    SingleEntryType pFuncSingleEntryType2 = (SingleEntryType)(&TestPointer2::PrintPointer2);

    pFuncSingleEntryType1(&msg1);
    pFuncSingleEntryType2(&msg2);

    VisitorCallable::EntryType pFuncEntryType1 = (VisitorCallable::EntryType )&TestPointer1::PrintPointer1;
    VisitorCallable::EntryType pFuncEntryType2 = (VisitorCallable::EntryType )&TestPointer2::PrintPointer2;

    (( (VisitorCallable*)(&t1))->*pFuncEntryType1)(&msg1);

    // (VisitorCallable*)(&t1)->*(pFuncEntryType1)(&msg1);
    // t2->pFuncEntryType2(&msg2);

    // pFuncEntryType1(&msg1);
    // pFuncEntryType2(&msg2);

    // VisitorCallable::*EntryType
}

