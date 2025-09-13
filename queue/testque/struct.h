#pragma once

#include <memory>
#include <random>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>

#include "json_util.hpp"
#include "base_util.h"

using namespace std;

#define MAX_POP_TIME 1000000000*3600 // 1h

struct MetaData {
    unsigned int iMemMBSize;
    unsigned int iWriteThreadCount;
    unsigned int iReadThreadCount;
    unsigned int iSleepTimeUs;    
    unsigned int iReadType;
    unsigned int iWriteSecs;
    unsigned int iWriteBlockCount;

    string str() {
        return "iMemMBSize=" + std::to_string(iMemMBSize) +
               ",iWriteThreadCount=" + std::to_string(iWriteThreadCount) +
               ",iReadThreadCount=" + std::to_string(iReadThreadCount) +
               ",iSleepTimeUs=" + std::to_string(iSleepTimeUs) +
               ",iReadType=" + std::to_string(iReadType) +
               ",iWriteSecs=" + std::to_string(iWriteSecs) +
               ",iWriteBlockCount=" + std::to_string(iWriteBlockCount);
    }

    bool InitFromJson(njson& jsonSrc, string& sErrMsg) {
        if (!GetJsonUnsignedIntField(jsonSrc, "iMemMBSize", iMemMBSize, sErrMsg)) {
            return false;
        }
        if (!GetJsonUnsignedIntField(jsonSrc, "iWriteThreadCount", iWriteThreadCount, sErrMsg)) {
            return false;
        }
        if (!GetJsonUnsignedIntField(jsonSrc, "iReadThreadCount", iReadThreadCount, sErrMsg)) {
            return false;
        }
        if (!GetJsonUnsignedIntField(jsonSrc, "iSleepTimeUs", iSleepTimeUs, sErrMsg)) {
            return false;
        }
        if (!GetJsonUnsignedIntField(jsonSrc, "iReadType", iReadType, sErrMsg)) {
            return false;
        }
        if (!GetJsonUnsignedIntField(jsonSrc, "iWriteSecs", iWriteSecs, sErrMsg)) {
            return false;
        }
        if (!GetJsonUnsignedIntField(jsonSrc, "iWriteBlockCount", iWriteBlockCount, sErrMsg)) {
            return false;
        }   
        return true;
    }
};


struct DataBlock {
    unsigned long long push_time_; // 数据块开始时间
    unsigned long long pop_time_; // 数据块开始时间
    unsigned int  size_; // 数据块大小
    unsigned char base_data_[2]; // 数据块指针
    unsigned int  array_size_; // 数据块数组大小
    DataBlock() {
        push_time_ = 0;
        pop_time_ = 0;
        size_ = 0;
        array_size_ = 2;
        for(int i=0;i<array_size_;i++){
            base_data_[i] = i%128;
        }        
    }

    DataBlock(DataBlock* pBlock) {
        size_ = pBlock->size_;
        push_time_ = pBlock->push_time_;
        pop_time_ = pBlock->pop_time_;
        array_size_ = pBlock->array_size_;

        CopyData(pBlock->GetData());
    }    

    // 赋值运算符：深拷贝（注意处理自赋值）
    DataBlock& operator=( DataBlock& other) {
        if (this == &other) return *this; // 避免自赋值
        std::cout << "DataBlock::operator=" << std::endl; 
        size_ = other.size_;
        push_time_ = other.push_time_;
        pop_time_ = other.pop_time_;
        array_size_ = other.array_size_;
        // CopyData((&other)->GetData());
        // memcpy(GetData(), (&other)->GetData(), array_size_);
        return *this;
    }    

    virtual ~DataBlock() {}

    void CopyBlock(DataBlock* pBlock, bool IsNeedSetPushTime=true) {    
        size_ = pBlock->size_;
        
        pop_time_ = pBlock->pop_time_;
        array_size_ = pBlock->array_size_;
        CopyData(pBlock->GetData());

        if(IsNeedSetPushTime){
            push_time_ = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            pBlock->push_time_ = push_time_;
        } else {
            push_time_ = pBlock->push_time_;
        }
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(base_data_, pSrcData, array_size_);
    }

    virtual unsigned char* GetData() {
        return base_data_;
    }

};

struct DataBlock1:public DataBlock {
    DataBlock1() {
        array_size_ = 4;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }
    }

    unsigned char data_[4]; // 数据块

    virtual unsigned char* GetData() {
        // std::cout << "DataBlock1::GetData" << std::endl;
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }    
};

struct DataBlock2:public DataBlock {
    DataBlock2() {
        array_size_ = 8;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }
    }

    unsigned char data_[8]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock3:public DataBlock {
    DataBlock3() {
        array_size_ = 16;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }
    }

    unsigned char data_[16]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock4:public DataBlock {
    DataBlock4() {
        array_size_ = 32;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }
    }
 
    unsigned char data_[32]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock5:public DataBlock {
    DataBlock5() {
        array_size_ = 64;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }
    }
  
    unsigned char data_[64]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock6:public DataBlock {
    DataBlock6() {
        array_size_ = 128;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }
    }

    unsigned char data_[128]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock7:public DataBlock {
    DataBlock7() {
        array_size_ = 256;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }
    }

    unsigned char data_[256]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock8:public DataBlock {
    DataBlock8() {
        array_size_ = 512;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }        
    }

    unsigned char data_[512]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock9:public DataBlock {
    DataBlock9() {
        array_size_ = 1024;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }        
    }

    unsigned char data_[1024]; // 数据块
    
    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock10:public DataBlock {
    DataBlock10() {
        array_size_ = 2048;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }        
    }
 
    unsigned char data_[2048]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock11:public DataBlock {
    DataBlock11() {
        array_size_ = 4096;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }        
    }

    unsigned char data_[4096]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};

struct DataBlock12:public DataBlock {
    DataBlock12() {
        array_size_ = 8192;
        for(int i=0;i<array_size_;i++){
            data_[i] = i%128;
        }        
    }
 
    unsigned char data_[8192]; // 数据块

    virtual unsigned char* GetData() {
        return data_;
    }

    virtual void CopyData (unsigned char* pSrcData) {
        memcpy(data_, pSrcData, size_);
    }      
};



using DataBlockPtr = std::shared_ptr<DataBlock>; 

DataBlockPtr GetRandomDataBlock() ;

DataBlockPtr GetCopyBlock(DataBlockPtr pBlockShptr);

DataBlockPtr GetCopyBlock(DataBlock* pBlock);

void CopyDataBlockToBuffer(char* pDstBuffer, DataBlock* pSrcBlock);

void test_struct();