#pragma once

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include <string>
#include <iostream>
using std::string;
using std::cout;
using std::endl;




class DBEngine
{
    public:

        DBEngine(string address, string usr, string pwd):
        usr_{usr}, pwd_{pwd}, host_{address}
        {

        }

        DBEngine(string usr, string pwd, string schema, int port, string host):
         usr_{usr}, pwd_{pwd}, schema_{schema}, port_{port}, host_{host} 
        {
            connect_mysql_schema();
        }        

        void connect_mysql();

        void connect_mysql_schema();

        void create_db(string db_name);

        bool check_db(string db_name);

        // void create_table(string table_name, string sql_str);

        void create_table(string account_name);

    private:

    sql::Connection*        conn_{NULL};
    sql::Statement*         state_;

    string                  usr_;
    string                  pwd_;
    string                  host_;
    string                  schema_;
    int                     port_;


    

};

void TestEngine();