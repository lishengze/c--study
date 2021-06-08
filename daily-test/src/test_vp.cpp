#include "test_vp.h"

void test_vp()
{
    A a;

    // cout << hex;
    cout << "address of a: " << &a << endl;
    cout << "address of vtbl: " << *(int*)(&a) << endl;
 
	// int faddr = *(int *)*(int *)(&a);//f()地址
 
    //int addr1 = *(int *)(*(int *)(&a)+4); //f1()地址，偏移4个字节直接到f1()
	// int f1addr = *((int *)(*(int *)(&a))+ 1); // 或者以4个字节作为整体 偏移1到f1()
 
	// int f2addr = *((int *)(*(int *)(&a)) + 2);  //f2()地址

    // cout << "address of f " << faddr << endl;
    // cout << "address of f1 " << f1addr << endl;
    // cout << "address of f2 " << f2addr << endl;


    // ((void(*)(void))faddr)();  //通过f()的地址调用f()
    
    // ((void(*)(void))f1addr)(); //通过f1()的地址调用f1()
    
    // ((void(*)(void))f2addr)(); //通过f2()的地址调用f2()
}



void Server::start()
{
    try
    {
        if (a_)
        {
            a_->f();
        }
        else
        {
            cout << "a_ is null " << endl;
        }
        // thread_ = std::thread(&Server::thread_main, this);

        thread_ = new thread(&Server::thread_main, this);

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void Server::thread_main()
{
    try
    {
        cout << "thread_main" << endl;
        if (a_)
        {
            a_->f();
        }
        else
        {
            cout << "a_ is null " << endl;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

void test_poly()
{
    B* b = new B();
    Server* server = new Server();
    server->regiser_A(b);
    server->start();

    std::this_thread::sleep_for(std::chrono::seconds(10));

    delete b;
    delete server;
}

void test_vp_main()
{
    test_poly();
}