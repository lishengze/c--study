#include <iostream>
#include <thread>
#include <chrono>
#include "func1.h"

int main() {    

    func1();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}