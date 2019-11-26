#ifndef _HUNGRY_SINGLETON_H_
#define _HUNGRY_SINGLETON_H_

#include <mutex>
using std::mutex;

class HungrySingleton
{
    public:
        static HungrySingleton* Instance();

        static HungrySingleton* m_sInstance;

    private:
        HungrySingleton();
        int                    m_value;
        static mutex           m_mutex;
};

#endif // !_HUNGRY_SINGLETON_H_
