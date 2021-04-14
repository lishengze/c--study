#include "DBEngine.h"

#include <boost/smart_ptr.hpp>
#include <boost/scoped_ptr.hpp>

#include "sql_scripts.h"

void DBEngine::connect_mysql()
{
    try
    {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        conn_ = driver->connect(host_, usr_, pwd_);
        
        if (conn_ == NULL)
        {
            cout << "conn is null" << endl;
        }
        cout << "connect suceess" << endl;    
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}


void DBEngine::connect_mysql_schema()
{
    try
    {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();

        sql::ConnectOptionsMap connection_properties;
        connection_properties["hostName"] = host_;
        connection_properties["userName"] = usr_;
        connection_properties["password"] = pwd_;
        connection_properties["schema"] = schema_;
        connection_properties["port"] = port_;
        connection_properties["OPT_RECONNECT"] = true;

        conn_ = driver->connect(connection_properties);
        
        if (conn_ == NULL)
        {
            cout << "conn is null" << endl;
        }
        cout << "connect suceess" << endl;  
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void DBEngine::create_table(string account_name)
{
    try
    {
        boost::scoped_ptr<sql::Statement> stmt1(conn_->createStatement());
        stmt1->execute(get_req_create_order_sql_str(account_name));

        boost::scoped_ptr<sql::Statement> stmt2(conn_->createStatement());
        stmt2->execute(get_rsp_create_order_sql_str(account_name));

        boost::scoped_ptr<sql::Statement> stmt3(conn_->createStatement());
        stmt3->execute(get_rtn_order_sql_str(account_name));

        boost::scoped_ptr<sql::Statement> stmt4(conn_->createStatement());
        stmt4->execute(get_rtn_trade_sql_str(account_name));

        boost::scoped_ptr<sql::Statement> stmt5(conn_->createStatement());
        stmt5->execute(get_req_create_order_sql_str(account_name));

        boost::scoped_ptr<sql::Statement> stmt6(conn_->createStatement());
        stmt6->execute(get_rsp_cancel_order_sql_str(account_name));        

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

// bool DBEngine::check_db(string db_name)
// {
//     try
//     {
//         bool result = false;

//         string check_db_sql = string("select * from information_schema.SCHEMATA where SCHEMA_NAME = '") + db_name + "'; ";

//         sql::Statement* state;
//         sql::ResultSet* sql_result;

//         cout << "check_db_sql: " << check_db_sql << endl;

//         if (!conn_)
//         {
//             std::cout << "Database was not connected!" << std::endl;
//             return result;
//         }

//         state = conn_->createStatement();

//         sql_result = state->executeQuery(check_db_sql);

//         if (!sql_result)
//         {
//             cout << "db: " << " does not exits!" << endl;
//             return false;
//         }
//         else
//         {
//             result = !(sql_result->wasNull());
//         }

//         cout << db_name << " " << result << endl;

//         return result;
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }
    
// }

bool DBEngine::check_db(string db_name)
{
    try
    {
        bool result = false;

        string get_all_db_sql = "show databases;";

        sql::Statement* state;
        sql::ResultSet* sql_result;

        cout << "get_all_db_sql: " << get_all_db_sql << endl;

        if (!conn_)
        {
            std::cout << "Database was not connected!" << std::endl;
            return result;
        }

        state = conn_->createStatement();

        sql_result = state->executeQuery(get_all_db_sql);

        if (!sql_result)
        {
            cout << "db: " << " does not exits!" << endl;
            return false;
        }
        else
        {
            std::cout << "All DB: " << std::endl;
            while(sql_result->next())
            {
                // std::cout << result->getString(0).c_str() << " " << result->getString(1).c_str() << " " << result->getString(2).c_str() << std::endl;

                std::cout << sql_result->getString(1).c_str() << std::endl;

                if (db_name == sql_result->getString(1).asStdString())
                {
                    result = true;
                    break;
                }

                // std::cout << ++i << std::endl;
            }
            std::cout << std::endl;
        }

        cout << db_name << " " << result << endl;

        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

void DBEngine::create_db(string db_name)
{
    try
    {
        // string sql_str = "create databss";

        // string check_db_sql = string("select * from information_schema.SCHEMATA where SCHEMA_NAME = '") + db_name + "'; ";
        
        // string get_all_db_sql = "select * from information_schema.SCHEMATA;";

        // // string get_all_db_sql = "show databases;";


        // sql::Statement* state;
        // sql::ResultSet* result;

        // cout << "get_all_db_sql: " << get_all_db_sql << endl;

        // if (!conn_)
        // {
        //     std::cout << "Database was not connected!" << std::endl;
        //     return;
        // }

        // state = conn_->createStatement();
        // result = state->executeQuery(get_all_db_sql);

        // int i = 0;
        
        // while(result->next())
        // {
        //     // std::cout << result->getString(0).c_str() << " " << result->getString(1).c_str() << " " << result->getString(2).c_str() << std::endl;

        //     std::cout << result->getString(1).c_str() << std::endl;

        //     std::cout << ++i << std::endl;
        // }

        if (!check_db(db_name))
        {
            string create_db_sql = "create database " + db_name + ";";
            sql::ResultSet* result;

            cout << "create_db_sql: " << create_db_sql << endl;

            sql::Statement* state = conn_->createStatement();
            bool exc_result = state->execute(create_db_sql);
            if (!exc_result)
            {
                cout << " create db " << db_name << " failed!" << endl;
            }
            else
            {
                cout << "Set Schema: " << db_name << endl;
                conn_->setSchema(db_name);                
            }

            // cout << "create over" << endl;

            // if (check_db(db_name))
            // {
            //     cout << "Set Schema: " << db_name << endl;
            //     conn_->setSchema(db_name);
            // }

            // cout << "check over" << endl;

            // while(result->next())
            // {
            //     std::cout << result->getString(1).c_str() << std::endl;
            // }

            // conn_->setSchema(db_name);
        }
        else
        {
            cout << "Set Schema: " << db_name << endl;
            conn_->setSchema(db_name);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}

// void DBEngine::create_table(string table_name, string sql_str)
// {

// }


class TestDBEngine
{
    public:
        TestDBEngine()
        {
            // test_connect();

            test_connect_schema();
        }

        void test_connect()
        {
            string address = "tcp://127.0.0.1:3306";
            string usr = "wormhole";
            string pwd = "worm";

            DBEngine db(address, usr, pwd);
            db.connect_mysql();
            db.create_db("TestTrade17");
        }

        void test_connect_schema()
        {
            string host = "127.0.0.1:3306";
            string usr = "wormhole";
            string pwd = "worm";
            string schema = "TestTrade";
            int    port = 3306;

            DBEngine db(usr, pwd, schema, port, host);
            // db.connect_mysql();
            // db.create_db("TestTrade17");            
        }
};

void TestEngine()
{
    TestDBEngine test_obj;
}