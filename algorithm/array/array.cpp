/*
author: lsz
date: 2025-11-26
description: test array
version: 1.0
log: 
    1. 初始化
    2. 增加二分法查找的逻辑；
*/

#include "array.h"
#include <vector>
using std::vector;




int BinaarySearchRecursion(std::vector<int>& srcVec, int left, int right, int target) {
    if (left > right) return -1;

    int mid = left + ((right - left) >> 1); 
    if (target == srcVec[mid]) return mid;

    if (target < srcVec[mid]) {
        return BinaarySearchRecursion(srcVec, left, mid - 1, target);
    } else {
        return BinaarySearchRecursion(srcVec, mid + 1, right, target);
    }

}

int BinaarySearchRecursion(std::vector<int>& srcVec, int target) {
    return BinaarySearchRecursion(srcVec, 0, srcVec.size() - 1, target);
}

int BinaarySearch(std::vector<int>& srcVec, int target) {
    int left = 0;
    int right = srcVec.size() - 1;
    while (left <= right) {
        int mid = left + ((right - left) >> 1); 
        if (target == srcVec[mid]) return mid;

        if (target < srcVec[mid]) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return -1;
}


void test_binary_search() {
    std::vector<int> srcVec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int target = 5;
    int index = BinaarySearch(srcVec, target);
    
    if (index == 4) {
        std::cout << "binary search success" << std::endl;
    } else {
        std::cout << "binary search failed" << std::endl;
    }

    index = BinaarySearchRecursion(srcVec, target);
    if (index == 4) {
        std::cout << "binary search recursion success" << std::endl;
    } else {
        std::cout << "binary search recursion failed" << std::endl;
    }
}


void TestArray() {
    test_binary_search();
}