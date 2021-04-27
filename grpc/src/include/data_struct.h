#pragma once

#include "../include/global_declare.h"

struct Fruit
{
    SessionType session_id;
    RpcType     rpc_id{"fruit"};
    string      request_id;
};

struct Apple:public Fruit
{
    Apple() { rpc_id = "apple";}

    string      name;
    string      time;
};

struct Pear:public Fruit
{
    Pear() { rpc_id = "pear";}

    string      name;
    string      time;
};

struct Mango:public Fruit
{
    Mango() { rpc_id = "mango";}

    string      name;
    string      time;
};
