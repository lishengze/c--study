#include "test.h"
#include "test_data_structure.h"
#include "test_string.h"

#include "global_declare.h"
#include "json.hpp"

#include "test_vp.h"

void test_json()
{
    try
    {    
        cout << "Test Parse Json " << endl;
        string file_name = "./test.json";
        std::ifstream in_config(file_name);

        if (!in_config.is_open())
        {
            cout << "Failed to Open: " << file_name << endl;
        }
        else
        {
            string contents((istreambuf_iterator<char>(in_config)), istreambuf_iterator<char>());

            cout << "Json Str: \n" << contents << endl;

            nlohmann::json js = nlohmann::json::parse(contents);        

            cout << js["BTC_USDT"]["depth"] << endl;  
        }
    
    }
    catch(const std::exception& e)
    {
        std::cerr << "Config::load_config: " << e.what() << '\n';
    }
}

void TestMain()
{
    // test_json();

    // test_data_structure();

    // TestString();q

    test_vp_main();
}