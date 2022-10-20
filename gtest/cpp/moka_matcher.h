#pragma once
#include "global_declare.h"

class Foo {

public:

   virtual bool Transform() = 0;

   // Overloaded on the types and/or numbers of arguments.

   virtual int Add(char x) = 0;

   virtual int Add(int x,int y) = 0;

   virtual int Add(int times, char x) = 0;

   

protected:

    virtual void Resume() = 0;

private:

    virtual int GetTimeOut() = 0;

};

class MockFoo : public Foo {

public:

    MOCK_METHOD0(Transform, bool());

    // The following must be in the public section, even though the

    // methods are protected or private in the base class.

    MOCK_METHOD0(Resume, void());

    MOCK_METHOD0(GetTimeOut, int());

    //virtual int Add(char x);

    //virtual int Add(int times, char x);

    // virtual int Add(int x);

    MOCK_METHOD1(Add, int(char x));

    //MOCK_METHOD1(Add, int(int x));

    MOCK_METHOD2(Add, int(int times, char x));

    // virtual int Add(int x,int y) = 0;

    MOCK_METHOD2(Add, int(int x, int y));

};
