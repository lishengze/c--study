#pragma once

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

void TestPointer();
