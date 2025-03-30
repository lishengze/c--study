#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
using namespace std;

module; // This line is added to indicate that this file is a module

export module my_module; // This line is added to indicate that this file is a modulized library

export void my_function() // This line is added to export the my_function() function
{
    cout << "Hello, world!" << endl;
}