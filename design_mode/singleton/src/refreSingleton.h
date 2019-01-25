#ifndef _REFRE_SINGLETON_H_
#define _REFRE_SINGLETON_H_

#include <mutex>
using std::mutex;

class RefreSingle
{
    public:
        static RefreSingle& Instance();

        int getValue();

    private:
        RefreSingle();

        int         m_value;
        mutex       m_mutex;
};

#endif // !_REFRE_SINGLETON_H_
