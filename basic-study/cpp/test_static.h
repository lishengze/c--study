#pragma once


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

void TestStaticMain();