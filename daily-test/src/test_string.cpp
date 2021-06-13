#include "test_string.h"

void test_erase()
{
    string origin_symbol = "BTC_USDT";

    cout << "symbol: " << origin_symbol << endl;

    if (origin_symbol.find("-") != std::string::npos)
    {
        origin_symbol.erase(origin_symbol.find("-"), 1);
    }
    
    if (origin_symbol.find("_") != std::string::npos)
    {
        origin_symbol.erase(origin_symbol.find("_"), 1);
    }
        
    cout << "TransSymbol: " << origin_symbol << endl;
}

void TestString()
{
    test_erase();
}