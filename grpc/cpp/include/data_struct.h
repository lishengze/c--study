#pragma once

#include "global_declare.h"
#include "package_simple.h"


struct Fruit:public PacakgeBaseData
{
    RpcType     rpc_id{"fruit"};
    string      message;
};
DECLARE_PTR(Fruit);

struct Apple:public Fruit
{
    Apple(string name_value, string time_value):
    name{name_value}, time{time_value}
    { rpc_id = "apple";}
    string      name;
    string      time;
};
DECLARE_PTR(Apple);

struct Pear:public Fruit
{
    Pear() { rpc_id = "pear";}

    string      name;
    string      time;
};
DECLARE_PTR(Pear);

struct Mango:public Fruit
{
    Mango() { rpc_id = "mango";}

    string      name;
    string      time;
};
DECLARE_PTR(Mango);
