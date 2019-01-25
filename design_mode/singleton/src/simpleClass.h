#ifndef _SIMPLE_CLASS_H_
#define _SIMPLE_CLASS_H_

class SimpleClass
{
    public:
        SimpleClass();

        static SimpleClass* Instance();

    private:
        int     m_value;

};

#endif // !_SIMPLE_CLASS__H
