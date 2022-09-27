#pragma once



class Atom
{
    public:
        Atom(int price, int time): m_price(price), m_time(time) {}

        bool operator < (const Atom& obj) const
        {
            if (this->m_price < obj.m_price || (this->m_price == obj.m_price && this->m_time < obj.m_time))
            {
                return false;
            }
            else
            {
                return true;
            }
        }

    public:
        int     m_price;
        int     m_time;
};

void test_operator();