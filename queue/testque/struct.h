#pragma once

#include <memory>
#include <random>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>

#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <atomic>
#include <sys/syscall.h>

#include "json_util.hpp"
#include "base_util.h"
#include "test_log.h"

using namespace std;

enum class ProcessStatus {
    Initing = 0,
    Running = 1,
    Stop = 2,
    WriteEnd = 3,
};

enum class ReadType {
    Simple = 0,     // 普通读取;
    Position = 1,        // 位置读取;
};

enum class QueueType {
    Both = 0,   // 两种队列都测试;
    Quant = 1,  // Quant 队列测试;
    Mpmc = 2,   // mpmc 队列测试;
};

enum class BlockType {
    DYNAMIC = 0,   // 动态分配内存;
    FixedStruct = 1,   // 固定大小数据块;
    FixedPOD = 2,   // 固定大小 POD 类型数据块;
};

enum class TestType {
    Both = 0,   // 读写都测;
    Write = 1,   // 压测写入;
    Read = 2,   // 压测读取;
};

template<typename T>
std::string VecStr(std::vector<T>& vecSrc) {
    std::string strRst = "";
    for (const T& atom:vecSrc) {
        strRst += std::to_string(atom) + ", ";
    }
    if (vecSrc.size() > 0) {
        strRst = strRst.substr(0, strRst.length()-2);
    }
    return strRst;
}

struct MetaData {
    unsigned int iMemMBSize;
    unsigned int iWriteThreadCount;
    unsigned int iReadThreadCount;
    unsigned int iSleepTimeUs;    
    unsigned int iReadType;
    unsigned int iWorkSecs;
    unsigned int iReadSecs;
    unsigned int iWriteBlockCount;
    unsigned int iFixedBlock;   // 0 - 可变，1 - 固定 -结构体, 2 - 固定 - unsigned long long 极限测试读写性能;
    unsigned int iQueueType;    // 1 - Quant 队列, 2 - mpmc 队列，0 - 两者都测试;
    unsigned int iCheckDetailValue; //  当 iFixedBlock 为 1 时起作用, 0 - 表示读取时不验证取出来的值, 1 - 表示读取时验证取出来的值;
    unsigned int iNumaNode; // 绑定的 NumaNode;
    unsigned int iTestType; // 测试类型;
    unsigned int iTestCount; // 测试次数;

    std::vector<int> vecWriteCpuList; // 写线程绑定的CPUID 列表;
    std::vector<int> vecReadCpuList; // 读线程绑定的CPUID 列表;

    MetaData() {
        Init();
    }

    string str() {
        return "iMemMBSize=" + std::to_string(iMemMBSize) + ",\n"
               "iWriteThreadCount=" + std::to_string(iWriteThreadCount) + ",\n"
               "iReadThreadCount=" + std::to_string(iReadThreadCount) + ",\n"
               "iSleepTimeUs=" + std::to_string(iSleepTimeUs) + ",\n"
               "iReadType=" + std::to_string(iReadType) + ",\n"
               "iWorkSecs=" + std::to_string(iWorkSecs) + ",\n"
               "iReadSecs=" + std::to_string(iReadSecs) + ",\n"
               "iWriteBlockCount=" + std::to_string(iWriteBlockCount) + ",\n"
               "iFixedBlock=" + std::to_string(iFixedBlock) + ",\n"
               "iQueueType=" + std::to_string(iQueueType) + ",\n"
               "iCheckDetailValue=" + std::to_string(iCheckDetailValue) + ",\n"
               "iNumaNode=" + std::to_string(iNumaNode) + ",\n"
               "iTestType=" + std::to_string(iTestType) + ",\n"
               "iTestCount=" + std::to_string(iTestCount) + ",\n"
               "vecWriteCpuList: " + VecStr<int>(vecWriteCpuList) + ",\n"
               "vecReadCpuList: " + VecStr<int>(vecReadCpuList) + ",\n";
    }

    void Init() {
        iMemMBSize = 0;
        iWriteThreadCount = 0;
        iReadThreadCount = 0;
        iSleepTimeUs = 0;
        iReadType = 0;
        iWorkSecs = 0;
        iReadSecs = 0;
        iWriteBlockCount = 0;
        iFixedBlock = 0;
        iQueueType = 0;
        iCheckDetailValue = 0; 
        iNumaNode = 0;
        iTestType = 0;
        iTestCount = 1;
        vecWriteCpuList.reserve(10);
        vecReadCpuList.reserve(10);
        vecWriteCpuList.clear();
        vecReadCpuList.clear();        
    }

    bool InitFromJson(njson& jsonSrc, string& sErrMsg) {
        Init();

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
        if (!GetJsonUnsignedIntField(jsonSrc, "iWorkSecs", iWorkSecs, sErrMsg)) {
            return false;
        }
        if (!GetJsonUnsignedIntField(jsonSrc, "iReadSecs", iReadSecs, sErrMsg)) {
            return false;
        }
        if (!GetJsonUnsignedIntField(jsonSrc, "iWriteBlockCount", iWriteBlockCount, sErrMsg)) {
            return false;
        }   
        if (!GetJsonUnsignedIntField(jsonSrc, "iFixedBlock", iFixedBlock, sErrMsg)) {
            return false;
        }          

        if (!GetJsonUnsignedIntField(jsonSrc, "iQueueType", iQueueType, sErrMsg, "", true)) {
            return false;
        }  

        if (!GetJsonUnsignedIntField(jsonSrc, "iCheckDetailValue", iCheckDetailValue, sErrMsg, "", true)) {
            return false;
        }       

        if (!GetJsonUnsignedIntField(jsonSrc, "iNumaNode", iNumaNode, sErrMsg, "", true)) {
            return false;
        }           

        if (!GetJsonUnsignedIntField(jsonSrc, "iTestType", iTestType, sErrMsg, "", true)) {
            return false;
        }        

        if (!GetJsonUnsignedIntField(jsonSrc, "iTestCount", iTestCount, sErrMsg, "", true)) {
            return false;
        }          

        if (iTestCount == 0) iTestCount = 1;

        // 若是测读取性能, 则将写入线程强制设置为1;
        if (iTestType == 2)     {
            iWriteThreadCount = 1;
        }

        if (jsonSrc["vecWriteCpuList"].is_array()) {
            for (njson::iterator it = jsonSrc["vecWriteCpuList"].begin(); it != jsonSrc["vecWriteCpuList"].end(); ++it) {           
                njson jsAtom = *it;        
                if (jsAtom.is_number_integer()) {
                    int iCpuID = jsAtom.get<int>(); 
                    vecWriteCpuList.push_back(iCpuID);
                } else {
                    TEST_LOG_WARN("vecWriteCpuList has non int value");
                }                 
            }  
        }

        if (jsonSrc["vecReadCpuList"].is_array()) {
            for (njson::iterator it = jsonSrc["vecReadCpuList"].begin(); it != jsonSrc["vecReadCpuList"].end(); ++it) {           
                njson jsAtom = *it;        
                if (jsAtom.is_number_integer()) {
                    int iCpuID = jsAtom.get<int>(); 
                    vecReadCpuList.push_back(iCpuID);
                } else {
                    TEST_LOG_WARN("vecReadCpuList has non int value");
                }                 
            }  
        }        

        return true;
    }
};

// 用于无睡眠写入时的并发量测试;
struct TestOutput {
    std::vector<unsigned long long> vecWriteBeforePushTimeList;
    std::vector<unsigned long long> vecWriteAfterPushTimeList;
    std::vector<int> vecWriteSlotList;

    std::vector<unsigned long long> vecReadBeforePopTimeList;
    std::vector<unsigned long long> vecReadAfterPopTimeList;  
    std::vector<int> vecReadSlotList;  

    std::vector<unsigned long long> vecCostTime; 

    unsigned long long ulWriteStartTime;
    unsigned long long ulWriteEndTime;
    unsigned long long ulReadStartTime;
    unsigned long long ulReadEndTime;

    // std::atomic<bool> bIsWriteEnd;

    unsigned int iBlockCount_;
    unsigned int iWorkSecs_;

    unsigned int iWriteCount_;
    unsigned int iReadCount_;

    TestOutput() {
        iBlockCount_ = 0;
        iWorkSecs_ = 0;
        ulWriteStartTime = 0;
        ulWriteEndTime = 0;
        ulReadStartTime = 0;
        ulReadEndTime = 0;

        iWriteCount_ = 0;
        iReadCount_ = 0;


        vecWriteBeforePushTimeList.clear();
        vecWriteAfterPushTimeList.clear();
        vecReadBeforePopTimeList.clear();
        vecReadAfterPopTimeList.clear();
        vecCostTime.clear();

        vecWriteSlotList.clear();
        vecReadSlotList.clear();      
    }

    TestOutput& operator=(const TestOutput& other){
        if (this == &other) return *this;
        // bIsWriteEnd = other.bIsWriteEnd.load(std::memory_order_relaxed);
        iBlockCount_ = other.iBlockCount_;
        Init(other.iBlockCount_, other.iWorkSecs_);
        return *this;
    }        


    TestOutput(unsigned int iBlockCount, unsigned int iWorkSecs) {
        Init(iBlockCount, iWorkSecs);      
    }

    void Init(unsigned int iBlockCount, unsigned int iWorkSecs) {

        iBlockCount_ = iBlockCount;
        iWorkSecs_ = iWorkSecs;

        if (iWorkSecs == 0) {
            
            vecWriteBeforePushTimeList.reserve(iBlockCount);
            vecWriteAfterPushTimeList.reserve(iBlockCount);
            vecReadBeforePopTimeList.reserve(iBlockCount);
            vecReadAfterPopTimeList.reserve(iBlockCount);
            vecCostTime.reserve(iBlockCount);

            vecWriteSlotList.reserve(iBlockCount);
            vecReadSlotList.reserve(iBlockCount);

            for (int i = 0; i < iBlockCount; ++i) {
                vecWriteBeforePushTimeList.push_back(0);
                vecWriteAfterPushTimeList.push_back(0);
                vecReadBeforePopTimeList.push_back(0);
                vecReadAfterPopTimeList.push_back(0);
                vecCostTime.push_back(0);
            }
        }


        ulWriteStartTime = 0;
        ulWriteEndTime = 0;
        ulReadStartTime = 0;
        ulReadEndTime = 0;             
    }


};

using TestOutputPtr = std::shared_ptr<TestOutput>; 

struct DataBlockFixed {
    unsigned long long push_time_; // 数据块开始时间
    unsigned long long pop_time_; // 数据块开始时间
    unsigned int  size_; // 数据块大小
    unsigned char data_[104]; // 数据块指针
    unsigned int  array_size_; // 数据块数组大小
    DataBlockFixed() {
        push_time_ = 0;
        size_ = 0;
        array_size_ = 104;
        for (int i = 0; i < 104; ++i) {
            data_[i] = i % 128;
        }
    }
    DataBlockFixed(const DataBlockFixed& other) {
        // printf("DataBlockFixed(const DataBlockFixed& other) \n");
        if (this == &other) return; // 避免自赋值
        push_time_ = other.push_time_;
        pop_time_ = other.pop_time_;
        size_ = other.size_;
        array_size_ = other.array_size_;
        memcpy(data_, other.data_, array_size_);
    }    

    DataBlockFixed& operator=(const DataBlockFixed& other) {
        if (this == &other) return *this; // 避免自赋值
        push_time_ = other.push_time_;
        pop_time_ = other.pop_time_;
        size_ = other.size_;
        array_size_ = other.array_size_;
        memcpy(data_, other.data_, array_size_);
        return *this;
    }    

};

using DataBlockFixedPtr = std::shared_ptr<DataBlockFixed>; 

DataBlockFixedPtr GetDataBlockFixed();

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

void CopyDataBlockToBuffer(char* pDstBuffer, char* pSrcBlock);

void BindCpuID(int iCpuID, int iNumaNode=0, string strMetaInfo="") ;

std::string NanoToMicroString(unsigned long long ulNanosecs);

std::string NanoToNanoString(unsigned long long ulNanosecs) ;

std::string GetAnaRst(std::vector<unsigned long long>& vecTime, MetaData metaData, unsigned long long costNanosecs=0, std::string sQueueName="");

std::string GetAnaRst(std::vector<unsigned long long>& vecTime, MetaData metaData, unsigned long long ulStartTime, unsigned long long ulEndTime,  std::string sQueueName="");


std::string GetAnaTestOutputRst(TestOutput& testOutput);

std::string GetAnaTestOutputTimeRst(TestOutput& testOutput, int iTestType);

std::string GetAnaTestOutputRst(std::vector<TestOutput>& vecWriteTestOutput, int iTestType);

void GetPushPopDelayVecCostTime(std::vector<TestOutput>& vecReadOutput, std::vector<unsigned long long >& vecCostTime);

void GetStartEndTimeFromWriteRead(std::vector<TestOutput>& vecWriteTestOutput, std::vector<TestOutput>& vecReadOutput,
                                   unsigned long long& ulStartTime, unsigned long long& ulEndTime);



void test_struct();