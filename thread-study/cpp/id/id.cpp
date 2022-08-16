#include "id.h"

void ID::init_id_list()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);

        for (int i = 0; i < id_count_; ++i)
        {
            // id_set_.push_back(max_id_++);

            id_set_.insert(max_id_++);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

ID_TYPE ID::get_new_id()
{
    try
    {
        while(id_set_.empty())
        {
            expand_id_list();
        }

        {
            std::lock_guard<std::mutex> lk(mutex_);

            ID_TYPE result = *(id_set_.begin());
            id_set_.erase(id_set_.begin());

            return result;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void ID::return_old_id(ID_TYPE old_id)
{
    try
    {
       std::lock_guard<std::mutex> lk(mutex_);

       if (id_set_.find(old_id) != id_set_.end())
       {
           id_set_.insert(old_id);
       }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void ID::expand_id_list()
{
    try
    {
        std::lock_guard<std::mutex> lk(mutex_);

        for (int i = 0; i < id_count_; ++i)
        {
            // id_set_.push_back(max_id_++);

            id_set_.insert(max_id_++);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void test_id()
{
    ID id_obj;

    for (int i =0; i < 10; ++i)
    {
        cout << "new id: " << id_obj.get_new_id() << endl;
    }
}

void TestIDMain()
{
    test_id();
}