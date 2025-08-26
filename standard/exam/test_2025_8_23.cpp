#include "test_2025_8_23.h"
#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <unordered_map>
#include <deque>
using namespace std;

void test1() {
    std::vector<int> v;
    v.push_back(100);
    v.push_back(300); 
    v.push_back(300); 
    v.push_back(500);
    
    std::vector<int>::iterator it;
    for(it = v.begin(); it!= v.end(); ++it) {
        if  (*it == 300) {  
            it = v.erase(it);
        }
    }

    for (it = v.begin(); it!= v.end(); ++it) {
        std::cout << *it << std::endl;
    }



}

void test2() {
    struct A {
        int a;
        union {
            long b;
            short c;
       };
    };

    struct B {
        char e;
        struct A a;
        int f;
    };
    cout << sizeof(B) << endl;

    class T{};
    cout << sizeof(T) << endl;
}

void test3() {
    int arr[5] = {1, 2, 3, 4, 5};
    cout << *(*(&arr + 1)-1) << endl;

}

void test4() {
    
}

void test5() {
    
}

void test6() {
    
}

void test7() {
    
}

void test8() {
    
}

void test9() {
    
}
#include <vector>
#include <stdexcept>
#include <algorithm>

using namespace std;

// 交换两个元素
void swap(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

// 对数组的一个子区间进行插入排序
void insertionSort(vector<int>& nums, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
        int key = nums[i];
        int j = i - 1;
        while (j >= left && nums[j] < key) {
            nums[j + 1] = nums[j];
            j--;
        }
        nums[j + 1] = key;
    }
}

// 找到数组子区间的中位数
int findMedian(vector<int>& nums, int left, int right) {
    insertionSort(nums, left, right);
    int length = right - left + 1;
    return nums[left + (length - 1) / 2]; // 返回中位数
}

// 中位数的中位数算法，用于确定性地选择基准元素
int medianOfMedians(vector<int>& nums, int left, int right) {
    int n = right - left + 1;
    
    // 如果数组长度小于等于5，直接返回中位数
    if (n <= 5) {
        return findMedian(nums, left, right);
    }
    
    // 将数组分成每组5个元素的小组
    // 存储每个小组的中位数
    vector<int> medians;
    
    for (int i = left; i <= right; i += 5) {
        int subRight = min(i + 4, right);
        int median = findMedian(nums, i, subRight);
        medians.push_back(median);
    }
    
    // 递归找到中位数数组的中位数
    return medianOfMedians(medians, 0, medians.size() - 1);
}

// 找到基准元素在数组中的位置
int findPivotIndex(vector<int>& nums, int left, int right, int pivot) {
    for (int i = left; i <= right; ++i) {
        if (nums[i] == pivot) {
            return i;
        }
    }
    return right; // 找不到时返回默认位置
}

// 分区操作，返回基准元素的最终位置
int partition(vector<int>& nums, int left, int right) {
    // 使用中位数的中位数算法选择基准
    int pivot = medianOfMedians(nums, left, right);
    // 找到基准在数组中的位置并交换到末尾
    int pivotIndex = findPivotIndex(nums, left, right, pivot);
    swap(nums[pivotIndex], nums[right]);
    
    // 标准分区过程
    pivot = nums[right];
    int i = left - 1;  // 大于等于基准区域的边界
    
    for (int j = left; j < right; j++) {
        // 如果当前元素大于等于基准，放入对应区域
        if (nums[j] >= pivot) {
            i++;
            swap(nums[i], nums[j]);
        }
    }
    
    // 将基准元素放到正确的位置
    swap(nums[i + 1], nums[right]);
    return i + 1;
}

// 确定性快速选择算法
int quickSelect(vector<int>& nums, int left, int right, int k) {
    if (left == right) {
        return nums[left];
    }
    
    // 进行分区操作
    int pivotIndex = partition(nums, left, right);
    
    // 如果基准元素的位置正好是第k个最大元素的位置
    if (pivotIndex == k - 1) {
        return nums[pivotIndex];
    } 
    // 如果基准元素在目标位置的左边，搜索右半部分
    else if (pivotIndex < k - 1) {
        return quickSelect(nums, pivotIndex + 1, right, k);
    } 
    // 否则搜索左半部分
    else {
        return quickSelect(nums, left, pivotIndex - 1, k);
    }
}

// 主函数：查找数组中第k个最大元素
int func(vector<int>& nums, int k) {
    // 异常处理
    if (nums.empty()) {
        throw invalid_argument("数组为空");
    }
    
    if (k <= 0 || k > static_cast<int>(nums.size())) {
        throw invalid_argument("k的值超出有效范围");
    }
    
    // 调用确定性快速选择算法
    return quickSelect(nums, 0, nums.size() - 1, k);
}
    void test10() {                
        vector<int> nums = {5, 2, 4, 6, 1, 3};
        int k = 3;
        int result = func(nums, k);
        cout << "第" << k << "大元素为：" << result << endl;
    }




void test_2025_8_23() {
    cout << "Test 2025_8_23" << endl;
    // test1();
    // test2();
    // test3();
    test10();
}