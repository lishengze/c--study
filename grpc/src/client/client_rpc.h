#pragma once

#include "../include/global_declare.h"

class AsyncClient;

class ClientBaseRPC
{
    public:
        ClientBaseRPC() { }

        virtual ~ClientBaseRPC() { }

        virtual void process();

        virtual void procceed();

        virtual void release();

    private:
        bool     is_request_data_updated_{true};

};


class ClientApplePRC:ClientBaseRPC
{
    public:
        ClientApplePRC() { }

        virtual ~ClientApplePRC() { }

        virtual void procceed();
    private:


};

