#include <iostream>
#include <set>
#include <mutex>
#include <thread>
#include <atomic>
using namespace std;

using ID_TYPE = unsigned long;

class ID
{
    public:
        ID()
        {
            init_id_list();
        }
        
        void init_id_list();

        ID_TYPE get_new_id();

        void return_old_id(ID_TYPE old_id);
        
        void expand_id_list();

    private:
        std::set<ID_TYPE>       id_set_;
        int                     id_count_{5000};

        std::atomic_uint64_t    max_id_{0};
        std::mutex              mutex_;
};

void TestIDMain();