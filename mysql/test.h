
#pragma once

#include "DBEngine.h"


class TestDBEngine
{
    public:
        TestDBEngine();

        void test_connect();

        void test_connect_schema();

        void test_create_table();

        void test_insert_data(DBEngine& db, string& account_name);

        void test_get_db_data(DBEngine& db, string& account_name);

        void test_trade_console();
};
