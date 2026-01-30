#pragma once

#include <cassert>
#include <cstddef>
#include <mutex>
#include <condition_variable>
#include <list>
#include <future>
#include <functional>

template<typename T>
class MemoryPool1 {
    MemoryPool1(int iCapacity = 100, int iExpandCapacity=50): pFreeListHead_(nullptr), iCapacity_(iCapacity), iExpandCapacity_(iExpandCapacity) {

        assert(iCapacity_ > 0);
        assert(iExpandCapacity_ > 0);

        // 初始化空闲列表
        for (int i = 0; i < iCapacity_; ++i) {
            DataNode* pNode = new DataNode();
            pNode->next = pFreeListHead_;
            pFreeListHead_ = pNode;
        }
    }

    struct DataNode {
        DataNode() : next(nullptr) {
            memset(data, 0, sizeof(T));
        }
        DataNode* next;
        char data[sizeof(T)];
    };    

    void Expand() {

        // 扩展空闲列表
        for (int i = 0; i < iExpandCapacity_; ++i) {
            DataNode* pNode = new DataNode();
            pNode->next = pFreeListHead_;
            pFreeListHead_ = pNode;
        }
    }


    template<typename... Args>
    T* Allocate(Args&&... args) {
        if (pFreeListHead_ == nullptr) {
            Expand();
        }

        DataNode* pNode = pFreeListHead_;
        pFreeListHead_ = pNode->next;

        pNode->data = new (pNode->data) T(std::forward<Args>(args)...);

        return  (T*)(pNode->data);
    
    }

    void Deallocate(T* p) {
        assert(p != nullptr);

        DataNode* pNode = (DataNode*)((char*)p - offsetof(DataNode, data));

        if ((char*)(pNode->data) != (char*)p) {
            assert(false);
        }
        pNode->next = pFreeListHead_;
        pFreeListHead_ = pNode;
    }



    private:
        DataNode* pFreeListHead_;
        int  iCapacity_;
        int iExpandCapacity_;
};