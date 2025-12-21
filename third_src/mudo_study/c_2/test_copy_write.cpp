#include "test_copy_write.h"

std::mutex              g_mutex;

class Foo
{
public:
    Foo()
    {
        id_ = s_id_++;
    }

    void doit() const;

    int id_{0};

    static int s_id_;
};

int Foo::s_id_ = 0;

typedef std::vector<Foo> FooList;
typedef boost::shared_ptr<FooList> FooListPtr;
FooListPtr g_fool_list;


void post(const Foo& foo)
{
    try
    {
        std::lock_guard<std::mutex> lk(g_mutex);

        if (!g_fool_list.unique())
        {
            g_fool_list.reset(new FooList(*g_fool_list));
            cout << "g_fool_list.unique() is false" << endl;
        }

        assert(g_fool_list.unique());
        g_fool_list->emplace_back(foo);
        cout << "Post emplace " << foo.id_ << endl;
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }    
}

void traverse()
{
    try
    {
        FooListPtr fool_list;
        {
            std::lock_guard<std::mutex> lk(g_mutex);

            fool_list = g_fool_list;
        }
                    
        std::lock_guard<std::mutex> lk(g_mutex);

        for (std::vector<Foo>::const_iterator it = fool_list->begin();
            it != fool_list->end(); ++it)
        {
            it->doit();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}


void Foo::doit() const
{
    Foo foo;
    post(foo);
}

void test_dead_lock()
{
    Foo foo;
    g_fool_list.reset(new FooList);
    post(foo);
    traverse();    
}

void TestCopyWrite()
{
    test_dead_lock();
}