#pragma once

#include "../include/thread_safe_singleton.hpp"
#include "../include/global_declare.h"
#define CONFIG ThreadSafeSingleton<Config>::DoubleCheckInstance("./config.json")

class Config
{
    public:
        Config(string file_name):file_name_(file_name)
        {
            load_config(file_name);                      
        }

        void set_file(string file_name)
        {
            file_name_ = file_name;
            load_config(file_name);
        }

        void load_config(string file_name);

        string get_session_id() { return session_id_;}

        int get_test_count() { return test_count_;}

    private:
        string                  file_name_;
        string                  session_id_;
        int                     test_count_{10};
};