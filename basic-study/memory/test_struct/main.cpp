#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include <thread>
#include <chrono>


struct Atom {
    char m_name[1024*1024];
    Atom() {
        for (int i = 0; i < sizeof(m_name); ++i) {
            m_name[i] = i % 256;
        }
        // memset(m_name, 1, sizeof(m_name));
    }
};

struct Data {
    Data() {
        Atom tmp;
        for (int i = 0; i < 1000; ++i) {
            atoms.push_back(tmp);
        }
    }
    int a;
    
    std::vector<Atom> atoms;
};

void test (int iTestCount = 1000) {

    for (int i = 0; i < iTestCount; ++i)
    {
        std::shared_ptr<Data> data = std::make_shared<Data>();

        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        std::cout << "i: " << i << std::endl;
    }
}

int main() {
    test();
    return 0;
}