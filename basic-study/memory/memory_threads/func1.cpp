#include "func1.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>



struct Node {
    // Node () {
    //     memset(a, 0, sizeof(a));
    // }
  int a[1024];       // List of immediate predecessor nodes in graph
};

// void threa_func(int n, int size) {
//     std::cout << "threa_func start, n: " << n << ", size: " << size << ", thread id: " << std::this_thread::get_id() << std::endl;
//     for (int i = 0; i < n; ++i) {
//         Node* p = new Node[size];
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }
// }

// 1. 在threa_func中保存分配的指针，避免被编译器优化掉
void threa_func(int n, int size) {
    std::vector<Node*> pointers;
    std::cout << "threa_func start, n: " << n << ", size: " << size << ", thread id: " << std::this_thread::get_id() << std::endl;
    for (int i = 0; i < n; ++i) {
        Node* p = new Node[size];
        pointers.push_back(p);  // 保存指针，避免编译器优化
        // 实际写入内存，确保操作系统分配物理内存
        for (int j = 0; j < size; ++j) {
            p[j].a[0] = i + j;  // 写入实际数据
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // 不释放内存，保持内存增长
    // for (auto p : pointers) delete[] p;
}



void func1() {
    std::cout << "func1 start" << std::endl;
    std::thread th1(threa_func, 1000000, 2);
    std::thread th2(threa_func, 1000000, 4);
    std::thread th3(threa_func, 1000000, 8);
    th1.join();
    th2.join();
    th3.join();
    std::cout << "func1 end" << std::endl;
}

