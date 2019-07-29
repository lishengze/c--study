#include "test.h"
#include "file_process.h"
#include <thread>
#include <iostream>
using std::thread;
using std::cin;
using std::cout;
using std::endl;

void threadFunc()
{
    cout << "ThreadFunc: " << std::this_thread::get_id() << endl;
}

void testThread()
{
    thread tmpThread = thread(threadFunc);
    if (tmpThread.joinable())
    {
        tmpThread.join();
    }
}

void testMain()
{
    // testThread();

    testBasicAPI();
}