#ifndef TEST_STATIC_H
#define TEST_STATIC_H

class TestStaticClassFunc
{
    public:
        TestStaticClassFunc();

        static TestStaticClassFunc& Instance();

        void Init();

        void addValue(int addedValue);

        int getValue();

        static   int m_value;        
};

#endif