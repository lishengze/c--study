#include "config.h"
#include <fstream>
#include "../include/json.hpp"
#include "../include/log.h"
#include "../include/util.h"

using json=nlohmann::json;

void Config::load_config(string file_name)
{
    try
    {    
        std::ifstream in_config(file_name);

        if (!in_config.is_open())
        {
            cout << "Failed to Open: " << file_name << endl;
        }
        else
        {
            string contents((istreambuf_iterator<char>(in_config)), istreambuf_iterator<char>());

            cout << file_name_ << ": \n"
                 << contents 
                 << endl;

            nlohmann::json js = nlohmann::json::parse(contents);
            
            session_id_ = GET_JSON_STR_FIELD(js, "session_id");

            test_count_ = GET_JSON_NUM_FIELD(js, "test_count");

            cout << "session_id: " << session_id_ << ", test_count: " << test_count_ << endl;
        }
    
    }
    catch(const std::exception& e)
    {
        std::cerr << "Config::load_config: " << e.what() << '\n';
    }
}