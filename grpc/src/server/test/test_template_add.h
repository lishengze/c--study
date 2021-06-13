#pragma once

#include "../../include/global_declare.h"

struct TestDataA
{
    int id;
    int time;
    /* data */
};

struct TestDataB
{
    string id;
    string time;
    /* data */
};



class DataBase
{
public:
    virtual void add_data(TestDataA* data_a) { }
    virtual void add_data(TestDataB* data_a) { }

    virtual ~DataBase() {}
};

class DataUseA:public DataBase
{
public:
    virtual void add_data(TestDataA* data_a) {
        cout << "DataUseA add_data TestDataA " << endl;
    }

    virtual ~DataUseA() {}
};

class DataUseB:public DataBase
{
public:

    virtual void add_data(TestDataB* data_a) {

        cout << "DataUseB add_data TestDataB " << endl;
    }

    virtual ~DataUseB() {}

};

class User
{
public:

    template<class DataType>
    void add_data(DataType* data)
    {
        base_->add_data(data);
    }

    void init_base(DataBase* base)
    {
        base_ = base;
    }

    DataBase*    base_;
};


void TestTemplateAdd();