#include "test.h"
#include "threadsafe_singleton.h"

#include "Singleton.hpp"
#include "global_declare.h"

class Base
{
    public:
    Base()
    {
        cout << "This is Base" << endl;
    }

};

void testMain()
{
    // SingletonThreadSafe<Base>::GetInstance();

    // Singleton<Base>::DoubleCheckInstance();

    Singleton<Base>::Instance();
}