#include "struct.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <string>
#include <iostream>
#include <numa.h>

#include "base_util.h"

using namespace std;

DataBlockFixedPtr GetDataBlockFixed() {
    DataBlockFixedPtr data_block = std::make_shared<DataBlockFixed>();
    data_block->size_ = sizeof(DataBlockFixed);
    data_block->push_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return data_block;
}

DataBlockPtr GetRandomDataBlock() {
    std::random_device rd;  // 用于获取随机种子
    std::mt19937 gen(rd()); // 以随机设备作为种子的 Mersenne Twister 生成器

    // 设置随机数的范围
    std::uniform_int_distribution<> distrib(1, 3); // 1 到 12 之间的均匀分布
    // 生成随机数
    // int random_number = distrib(gen);
    int random_number = 1;
    DataBlockPtr data_block = nullptr;    

    switch (random_number) {
        case 1:
            data_block = std::make_shared<DataBlock1>();
            data_block->size_ = sizeof(DataBlock1);
            break;
        case 2:
            data_block = std::make_shared<DataBlock2>();
            data_block->size_ = sizeof(DataBlock2);
            break;
        case 3:
            data_block = std::make_shared<DataBlock3>();
            data_block->size_ = sizeof(DataBlock3);
            break;
        case 4:
            data_block = std::make_shared<DataBlock4>();
            data_block->size_ = sizeof(DataBlock4);
            break;  
        case 5:
            data_block = std::make_shared<DataBlock5>();
            data_block->size_ = sizeof(DataBlock5);
            break;
        case 6:
            data_block = std::make_shared<DataBlock6>();
            data_block->size_ = sizeof(DataBlock6);
            break;
        case 7:
            data_block = std::make_shared<DataBlock7>();
            data_block->size_ = sizeof(DataBlock7);
            break;
        case 8:
            data_block = std::make_shared<DataBlock8>();
            data_block->size_ = sizeof(DataBlock8);
            break;
        case 9:
            data_block = std::make_shared<DataBlock9>();
            data_block->size_ = sizeof(DataBlock9);
            break;
        case 10:
            data_block = std::make_shared<DataBlock10>();
            data_block->size_ = sizeof(DataBlock10);
            break;
        case 11:
            data_block = std::make_shared<DataBlock10>();
            data_block->size_ = sizeof(DataBlock10);
            break;
        case 12:
            data_block = std::make_shared<DataBlock12>();
            data_block->size_ = sizeof(DataBlock12);
            break;
        default:
            return nullptr;
    }

    // data_block->start_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return data_block;
}

DataBlockPtr GetDataBlockByArraySize(int array_size ) {
    if (array_size < 0 || array_size > 8192) {
        return nullptr;
    }

    DataBlockPtr data_block = nullptr;    

    switch (array_size) {
        case 4:
            data_block = std::make_shared<DataBlock1>();
            data_block->size_ = sizeof(DataBlock1);
            break;
        case 8:
            data_block = std::make_shared<DataBlock2>();
            data_block->size_ = sizeof(DataBlock2);
            break;
        case 16:
            data_block = std::make_shared<DataBlock3>();
            data_block->size_ = sizeof(DataBlock3);
            break;
        case 32:
            data_block = std::make_shared<DataBlock4>();
            data_block->size_ = sizeof(DataBlock4);
            break;  
        case 64:
            data_block = std::make_shared<DataBlock5>();
            data_block->size_ = sizeof(DataBlock5);
            break;
        case 128:
            data_block = std::make_shared<DataBlock6>();
            data_block->size_ = sizeof(DataBlock6);
            break;
        case 256:
            data_block = std::make_shared<DataBlock7>();
            data_block->size_ = sizeof(DataBlock7);
            break;
        case 512:
            data_block = std::make_shared<DataBlock8>();
            data_block->size_ = sizeof(DataBlock8);
            break;
        case 1024:
            data_block = std::make_shared<DataBlock9>();
            data_block->size_ = sizeof(DataBlock9);
            break;
        case 2048:
            data_block = std::make_shared<DataBlock10>();
            data_block->size_ = sizeof(DataBlock10);
            break;
        case 4096:
            data_block = std::make_shared<DataBlock11>();
            data_block->size_ = sizeof(DataBlock11);
            break;
        case 8192:
            data_block = std::make_shared<DataBlock12>();
            data_block->size_ = sizeof(DataBlock12);
            break;
        default:
            std::cout << "Unknonw array_size_: " << array_size << std::endl;
            return nullptr;
    }

    // data_block->start_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return data_block;
}

DataBlockPtr GetCopyBlock(DataBlockPtr pBlockShptr) {
    DataBlockPtr data_block2 = GetDataBlockByArraySize(pBlockShptr->array_size_);
    if (nullptr != data_block2) {
        data_block2->CopyBlock(pBlockShptr.get());
    }
    return data_block2;
}

DataBlockPtr GetCopyBlock(DataBlock* pBlock) {
    DataBlockPtr data_block2 = GetDataBlockByArraySize(pBlock->array_size_);
    if (nullptr != data_block2) {
        data_block2->CopyBlock(pBlock, false);
    }
    return data_block2;    
}

void CopyDataBlockToBuffer(char* pDstBuffer, char* pSrcBlock) {
    CopyDataBlockToBuffer(pDstBuffer, (DataBlock*)pSrcBlock);
}

void CopyDataBlockToBuffer(char* pBuffer, DataBlock* pSrcBlockOri) {
    // std::cout << "CopyDataBlockToBuffer " << pSrcBlockOri->array_size_ << ", size: " << pSrcBlockOri->size_ << std::endl;
    DataBlock* pDstBlock = nullptr;
    switch (pSrcBlockOri->array_size_) {
        case 4:{
            pDstBlock = new (pBuffer) DataBlock1();
            

            // DataBlock1* pSrcBlock = (DataBlock1*)pSrcBlockOri;
            //    std::cout << "Copy 4" << std::endl;
            //    DataBlock* pDstBlock = (DataBlock*)pBuffer;
            //    (* pDstBlock) = (* pSrcBlockOri);            
            //  (* pDstBlock) = (* pSrcBlock);

            // memcpy(pDstBlock->GetData(), pSrcBlockOri->GetData(), pSrcBlockOri->array_size_);

            // pDstBlock->size_ = pSrcBlockOri->size_;
            // 
            // pDstBlock->CopyBlock(pSrcBlockOri);
            // pDstBlock->GetData();
            // pSrcBlockOri->GetData();

            // memcpy(pDstBlock->GetData(), pSrcBlockOri->GetData(), pSrcBlockOri->array_size_);

            // std::cout << "Copy 4, data: " << (int)pDstBlock->data_[pSrcBlockOri->array_size_-1] << std::endl;
            break;
        }
        case 8:{
            pDstBlock = new (pBuffer) DataBlock2();
            break;
        }
        case 16:{
            pDstBlock = new (pBuffer) DataBlock3();
            break;
        }
        case 32:{
            pDstBlock = new (pBuffer) DataBlock4();
            break;
        }
        case 64:{
            pDstBlock = new (pBuffer) DataBlock5();
            break;
        }
        case 128:{
            pDstBlock = new (pBuffer) DataBlock6();
            break;
        }
        case 256:{
            pDstBlock = new (pBuffer) DataBlock7();
            break;
        }
        case 512:{
            pDstBlock = new (pBuffer) DataBlock8();
            break;
        }
        case 1024:{
            pDstBlock = new (pBuffer) DataBlock9();
            break;
        }
        case 2048:{
            pDstBlock = new (pBuffer) DataBlock10();
            break;
        }
        case 4096:{
            pDstBlock = new (pBuffer) DataBlock11();
            break;
        }
        case 8192:{
            pDstBlock = new (pBuffer) DataBlock12();
            break;
        }
        default:
            std::cout << "Unknonw array_size_: " << pSrcBlockOri->array_size_ << std::endl;
            break;
    }

    if (nullptr != pDstBlock) {
        pDstBlock->CopyBlock(pSrcBlockOri);
    }
}

void BindCpuID(int iCpuID, int iNumaNode, string strMetaInfo) {

    pid_t tid = syscall(SYS_gettid);

    // if (numa_set_preferred(iNumaNode) != 0) {
    //     TEST_LOG_WARN(strMetaInfo + " Thread: " + std::to_string(tid) + ",  Try Bind NumaNode: " + std::to_string(iNumaNode) + "Failed");
    // }

    if (numa_available() < 0) {
        TEST_LOG_WARN(strMetaInfo + " System does not support numa!");
    } else {
        int iTotalNodes = numa_num_configured_nodes();
        if (iNumaNode < 0 || iNumaNode >= iTotalNodes) {
            TEST_LOG_WARN(strMetaInfo + " iNumaNode " + std::to_string(iNumaNode) + " is illegal, MaxNumaNode is: " + std::to_string(iNumaNode));
        } else {
            numa_set_preferred(iNumaNode);
        }

    }

    // numa_set_preferred(iNumaNode);

    // struct bitmask tmp;
	// tmp.maskp = (unsigned long *)buffer;
	// tmp.size = buffer_len * 8;
	// return numa_node_to_cpus(node, &tmp);

    // if (!numa_bitmask_isbitset(numa_node_to_cpus(iNumaNode), iCpuID)) {
    //     TEST_LOG_DETAIL(strMetaInfo + " Thread: " + std::to_string(tid) 
    //                     + ", Try Bind NumaNode: " + std::to_string(iNumaNode)
    //                     + ", Bind CPU: " + std::to_string(iCpuID)  + "Failed");
    // }

    // std::cout << "Thread: " << tid  << " Start Running, Try Bind: " << iCpuID << std::endl;
    TEST_LOG_DETAIL(strMetaInfo + " Thread: " + std::to_string(tid) + ",  Try Bind CPU: " + std::to_string(iCpuID));

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(iCpuID, &mask);

    int ret = sched_setaffinity(tid, sizeof(mask), &mask);
    if (ret == -1) {
        TEST_LOG_ERROR(strMetaInfo + " Bind CPU: " + std::to_string(iCpuID) + " to thread: " + std::to_string(tid) + " Failed");
        return;
    }

    // TEST_LOG_WARN(strMetaInfo + " Bind CPU: " + std::to_string(iCpuID) + " to thread: " + std::to_string(tid) + " Sucess!");

    cpu_set_t get_mask;
    CPU_ZERO(&get_mask);
    sched_getaffinity(tid, sizeof(mask), &mask);
    for (int i = 0; i < CPU_SETSIZE; ++i) {
        if (CPU_ISSET(i, &get_mask)) {
            if (CPU_ISSET(i, &get_mask)) {
                TEST_LOG_WARN("Bind " + std::to_string(iCpuID) + " to " + std::to_string(tid) + " Sucess!");
                break;
            }
        }
    }
}

void test_struct() {
    DataBlockPtr data_block = GetRandomDataBlock();
    if (data_block) {
        unsigned char* pdata = data_block->GetData();
        std::cout << "Source data_block size: " << data_block->size_ << ", array_size_: " << data_block->array_size_ <<", " << (int)(pdata[data_block->array_size_-1])<< std::endl;
    }

    char* pCData = new char[data_block->size_];
    CopyDataBlockToBuffer(pCData, data_block.get());

    DataBlock* pdata_block = (DataBlock*)pCData;
    // (*pCData) = *(data_block->GetData());

    unsigned char* pArray = pdata_block->GetData();
    std::cout << "Copied To Buffer  data_block size: " << pdata_block->size_ << ", array_size_: " << pdata_block->array_size_ <<", " << (int)(pArray[pdata_block->array_size_-1])<< std::endl;
    

    DataBlockPtr data_block2 = GetCopyBlock(pdata_block);
    if (data_block2) {
        unsigned char* pdata = data_block2->GetData();
        std::cout << "Copy From Buffer data_block size: " << data_block2->size_ << ", array_size_: " << data_block2->array_size_ <<", " << (int)pdata[data_block2->array_size_-1]<< std::endl;
    }

    delete[] pCData;
}


std::string NanoToMicroString(unsigned long long ulNanosecs) {
    unsigned long long microSecs = ulNanosecs / 1000;

    time_t totalSecs = static_cast<time_t>(microSecs/1000000);
    int leftMicroSecs = static_cast<int>(microSecs%1000000);

    std::tm localTm = *std::localtime(&totalSecs);
    std::stringstream ss;
    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H-%M-%S", &localTm);

    ss << timeBuffer << "." << std::setw(6) << std::setfill('0') << leftMicroSecs;

    return ss.str();
}

std::string NanoToNanoString(unsigned long long ulNanosecs) {
    // unsigned long long microSecs = ulNanosecs / 1000;

    time_t totalSecs = static_cast<time_t>(ulNanosecs/NANO_PER_SECOND);
    unsigned long long leftMicroSecs = static_cast<int>(ulNanosecs%NANO_PER_SECOND);

    std::tm localTm = *std::localtime(&totalSecs);
    std::stringstream ss;
    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H-%M-%S", &localTm);

    ss << timeBuffer << "." << std::setw(9) << std::setfill('0') << leftMicroSecs;

    std::stringstream ss2;
    ss2 << std::setw(9) << std::setfill('0') << leftMicroSecs;

    std::string tmp = ss2.str();

    std::string rst = tmp.substr(0,3) + "," + tmp.substr(3,3) + "," + tmp.substr(6,3);


    return rst;
}

std::string GetAnaRst(std::vector<unsigned long long>& vecTime, MetaData metaData, 
                    unsigned long long costNanosecs, std::string sQueueName) {
    TEST_LOG_DETAIL("Start Ana Result: "+ sQueueName + ", ReadCount: " + std::to_string(vecTime.size()));
    unsigned long long ulEndNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    unsigned long long ulMin = 0; 
    unsigned long long ulMax = 0;
    unsigned long long ulAve = 0;
    std::sort(vecTime.begin(), vecTime.end());

    ulMin = vecTime[0];
    ulMax = vecTime[vecTime.size()-1];

    unsigned long long ul25 = vecTime[std::floor(vecTime.size()*25/100)];
    unsigned long long ul50 = vecTime[std::floor(vecTime.size()/2)];
    unsigned long long ul75 = vecTime[std::floor(vecTime.size()*75/100)];
    unsigned long long ul90 = vecTime[std::floor(vecTime.size()*9/10)];

    std::string sDelayTimeInfo = "costNanosecs: " + NanoStr(costNanosecs)
                        + ", dataCount: " + std::to_string(vecTime.size()) 
                        +  ", min=" + std::to_string(ulMin) + ", max=" + std::to_string(ulMax)
                        + ", 25%=" + std::to_string(ul25) + ", 50%=" + std::to_string(ul50) 
                        + ", 75%=" + std::to_string(ul75) + ", 90%=" + std::to_string(ul90);
    
    std::string sThroughput = "";
    if (costNanosecs > 0) {
        double dBlockSumCounts = vecTime.size();
        unsigned long long dBlockPerSecs = dBlockSumCounts * NANO_PER_SECOND / costNanosecs ;

        int BlockSize = 1;

        if (metaData.iFixedBlock == 1) {
            BlockSize = sizeof(DataBlockFixed);
        } else if (metaData.iFixedBlock == 2) {
            BlockSize = sizeof(unsigned long long );
        }
        unsigned long long dBytesPerSecs = dBlockPerSecs * BlockSize;

        sThroughput += "cost millsecs: " + std::to_string(costNanosecs/1000000)  + ", dBlockPerSecs: " 
                
                + std::to_string(dBlockPerSecs) + ", KB/S: " + std::to_string(dBytesPerSecs/1024) 
                + " KB";      
    }

    std::string strMsg =  "\nPush To Pop Delay Time: \n" + sDelayTimeInfo + "\n" + sThroughput + "\n\n";                                

    return strMsg;
}



std::string GetAnaTestOutputRst(TestOutput& testOutput, int iTestType) {
    std::vector<unsigned long long> vecPushIntervelList;
    std::vector<unsigned long long> vecPopIntervelList;

    std::string sStr = "\n";

    for (int i = 0; i < testOutput.vecReadBeforePopTimeList.size()-1; ++i) {
        sStr += NanoToNanoString(testOutput.vecWriteBeforePushTimeList[i]) + " | " 
            +  NanoToNanoString(testOutput.vecWriteAfterPushTimeList[i]) + " | "
            + std::to_string(testOutput.vecWriteAfterPushTimeList[i] - testOutput.vecWriteBeforePushTimeList[i]) + " | "
            // + std::to_string(testOutput.vecWriteAfterPushTimeList[i+1] - testOutput.vecWriteBeforePushTimeList[i]) + " | "
            + NanoToNanoString(testOutput.vecReadBeforePopTimeList[i]) + " | " 
            +  NanoToNanoString(testOutput.vecReadAfterPopTimeList[i]) + " | "
            + std::to_string(testOutput.vecReadAfterPopTimeList[i] - testOutput.vecReadBeforePopTimeList[i]) + " | "
            // + std::to_string(testOutput.vecReadAfterPopTimeList[i+1] - testOutput.vecReadBeforePopTimeList[i]) + " | "     
            + std::to_string(testOutput.vecReadBeforePopTimeList[i] - testOutput.vecWriteBeforePushTimeList[i]) + " | "  
            + std::to_string(testOutput.vecReadAfterPopTimeList[i] - testOutput.vecWriteBeforePushTimeList[i]) + " | "          
                    "\n";
    }
    return sStr;
}


std::string GetSimpleTimeData(std::vector<unsigned long long>& vecTime) {
    if (vecTime.size()  == 0) return  "";

    std::sort(vecTime.begin(), vecTime.end());

    unsigned long long ulMin = vecTime[0];
    unsigned long long ulMax = vecTime[vecTime.size()-1];

    unsigned long long ul25 = vecTime[std::floor(vecTime.size()*25/100)];
    unsigned long long ul50 = vecTime[std::floor(vecTime.size()/2)];
    unsigned long long ul75 = vecTime[std::floor(vecTime.size()*75/100)];
    unsigned long long ul90 = vecTime[std::floor(vecTime.size()*9/10)];

    std::string sDelayTimeInfo = ",dataCount: " + NanoStr(vecTime.size()) 
                        +  ", min=" + NanoStr(ulMin) + ", max=" + NanoStr(ulMax)
                        + ", 25%=" + NanoStr(ul25) + ", 50%=" + NanoStr(ul50) 
                        + ", 75%=" + NanoStr(ul75) + ", 90%=" + NanoStr(ul90)
                        + "\n";
    return sDelayTimeInfo;
        
}

std::string GetAnaTestOutputTimeRst(TestOutput& testOutput, int iTestType) {
    std::string sRst = "";

    if (iTestType != (int)TestType::Read) {
        std::vector<unsigned long long > vecPushCostTime;
        vecPushCostTime.reserve(testOutput.vecWriteBeforePushTimeList.size());

        if (testOutput.vecWriteBeforePushTimeList.size()>0) {
            vecPushCostTime.push_back(testOutput.vecWriteAfterPushTimeList[0] - testOutput.vecWriteBeforePushTimeList[0]);
        }

        for (int i = 1; i < testOutput.vecWriteBeforePushTimeList.size(); ++i) {
            vecPushCostTime.push_back(testOutput.vecWriteAfterPushTimeList[i] - testOutput.vecWriteAfterPushTimeList[i-1]);
        }

        unsigned long long costNano = testOutput.ulWriteEndTime - testOutput.ulWriteStartTime;

        std::string sTest="";
        int testCount = std::min(10, (int)(testOutput.vecWriteBeforePushTimeList.size()));
        // for (int i = 0; i < testCount; ++i) {
        //     sTest += NanoToMicroString(testOutput.vecWriteBeforePushTimeList[i]) + ", " + NanoToMicroString(testOutput.vecWriteAfterPushTimeList[i]) + "\n";
        // }        



        return std::string("costNano: ") + NanoStr(costNano)  + ", "+  GetSimpleTimeData(vecPushCostTime) + sTest;
    }

    if (iTestType != (int)TestType::Write) {
        std::vector<unsigned long long > vecPopCostTime;
        vecPopCostTime.reserve(testOutput.vecReadBeforePopTimeList.size());

        if (testOutput.vecReadBeforePopTimeList.size()>0) {
            vecPopCostTime.push_back(testOutput.vecReadAfterPopTimeList[0] - testOutput.vecReadBeforePopTimeList[0]);
        }

        for (int i = 1; i < testOutput.vecReadAfterPopTimeList.size(); ++i) {
            vecPopCostTime.push_back(testOutput.vecReadAfterPopTimeList[i] - testOutput.vecReadAfterPopTimeList[i-1]);
        }        

        unsigned long long costNano = testOutput.ulReadEndTime - testOutput.ulReadStartTime;

        std::string sTest="";
        int testCount = std::min(10, (int)(testOutput.vecReadBeforePopTimeList.size()));
        // for (int i = 0; i < testCount; ++i) {
        //     sTest += NanoToMicroString(testOutput.vecReadBeforePopTimeList[i]) + ", " + NanoToMicroString(testOutput.vecReadAfterPopTimeList[i]) + "\n";
        // }        

        return std::string("costNano: ") + NanoStr(costNano) + ", "+ GetSimpleTimeData(vecPopCostTime)+ sTest;
    }    
}

std::string GetAnaTestOutputRst(std::vector<TestOutput>& vecWriteTestOutput, int iTestType) {
    std::string sRst = "\n";
    if (vecWriteTestOutput.size() > 0) {

        unsigned long long ulReadStartTime =  vecWriteTestOutput[0].ulReadStartTime;
        unsigned long long ulReadEndTime = vecWriteTestOutput[0].ulReadEndTime;
        unsigned long long ulWriteStartTime = vecWriteTestOutput[0].ulWriteStartTime;
        unsigned long long ulWriteEndTime = vecWriteTestOutput[0].ulWriteEndTime;

        if (iTestType == 1) {
            sRst += "Write Push Time Ana: \n";
        }else if (iTestType == 2) {
            sRst += "Read Pop Time Ana: \n";
        }        
        
        int iSumWriteDataCount = 0;
        int iSumReadDataCount = 0;

        for(auto& tmp:vecWriteTestOutput) {
            sRst += GetAnaTestOutputTimeRst(tmp, iTestType);

            if (iTestType != (int)(TestType::Write)) {
                ulReadStartTime = ulReadStartTime <= tmp.ulReadStartTime ? ulReadStartTime : tmp.ulReadStartTime;
                ulReadEndTime = ulReadEndTime >= tmp.ulReadEndTime ? ulReadEndTime : tmp.ulReadEndTime;
                iSumReadDataCount += tmp.iBlockCount_;
            }


            if (iTestType != (int)(TestType::Read)) {
                ulWriteStartTime = ulWriteStartTime <= tmp.ulWriteStartTime ? ulWriteStartTime : tmp.ulWriteStartTime;
                ulWriteEndTime = ulWriteEndTime >= tmp.ulWriteEndTime ? ulWriteEndTime : tmp.ulWriteEndTime;  
                iSumWriteDataCount += tmp.iBlockCount_;
            }          
        }

        // sRst +="\n";

        if (iTestType != (int)(TestType::Write)) {
            sRst += "ReadStart: " + NanoToMicroString(ulReadStartTime) + ","
                 + "ReadEnd: " + NanoToMicroString(ulReadEndTime) + ","
                 + ", cost: " + NanoStr(ulReadEndTime - ulReadStartTime) 
                 + ", ave: " + NanoStr((ulReadEndTime - ulReadStartTime)/iSumReadDataCount)
                 
                 + "\n";
        }   

        if (iTestType != (int)(TestType::Read)) {
            sRst += "WriteStart: " + NanoToMicroString(ulWriteStartTime) + ","
                 + "WriteEnd: " + NanoToMicroString(ulWriteEndTime) + ","
                 + "cost: " + NanoStr(ulWriteEndTime - ulWriteStartTime)
                 + ", ave: " + NanoStr((ulWriteEndTime - ulWriteStartTime)/iSumWriteDataCount)
                 + "\n";
        }                              

    } else {
        sRst += "Empty!";
    }

    sRst += "\n";
    return sRst;
}


void GetStartEndTimeFromWriteRead(std::vector<TestOutput>& vecWriteTestOutput, std::vector<TestOutput>& vecReadOutput,
                                   unsigned long long& ulStartTime, unsigned long long& ulEndTime) {
    // std::cout << "vecWriteTestOutput.size: " << vecWriteTestOutput.size() << endl;
    if (vecWriteTestOutput.size() > 0 && vecReadOutput.size() > 0) {
        unsigned long long ulReadStartTime =  vecReadOutput[0].ulReadStartTime;
        unsigned long long ulReadEndTime = vecReadOutput[0].ulReadEndTime;

        unsigned long long ulWriteStartTime = vecWriteTestOutput[0].ulWriteStartTime;
        unsigned long long ulWriteEndTime = vecWriteTestOutput[0].ulWriteEndTime;

        // cout << "ulReadEndTime: "  << ulReadEndTime << ", ulWriteEndTime: " << ulWriteEndTime << endl;
           

        for(auto& tmp:vecWriteTestOutput) {
            ulWriteStartTime = ulReadStartTime <= tmp.ulWriteStartTime ? ulWriteStartTime : tmp.ulWriteStartTime;
            ulWriteEndTime = ulWriteEndTime >= tmp.ulWriteEndTime ? ulWriteEndTime : tmp.ulWriteEndTime;       
        }

        for(auto& tmp:vecReadOutput) {
            ulReadStartTime = ulReadStartTime <= tmp.ulReadStartTime ? ulReadStartTime : tmp.ulReadStartTime;
            ulReadEndTime = ulReadEndTime >= tmp.ulReadEndTime ? ulReadEndTime : tmp.ulReadEndTime;     
        }        

        

        ulStartTime = std::min(ulReadStartTime, ulWriteStartTime);
        ulEndTime = std::max(ulReadEndTime, ulWriteEndTime);

        // cout << "ulReadEndTime: "  << ulReadEndTime << ", ulWriteEndTime: " << ulWriteEndTime << endl;
    }


}

// 汇总所有读数据线程的 耗时统计数据;
void GetPushPopDelayVecCostTime(std::vector<TestOutput>& vecReadOutput, std::vector<unsigned long long >& vecCostTime) {
    vecCostTime.clear();
    // int i = 0;
    for(auto& tmp:vecReadOutput) {
        //  TEST_LOG_WARN("ReadCount: " + std::to_string(tmp.vecCostTime.size()) + ", firstTime: " + std::to_string(tmp.vecCostTime[0]));
        for (auto& time:tmp.vecCostTime) {
            if (time > 0) {
                // i++;
                vecCostTime.push_back(time);
            }
        }
    }

    // cout << "------ I: " << i << endl;
}

std::string GetAnaRst(std::vector<unsigned long long>& vecTime, MetaData metaData, 
                    unsigned long long ulStartTime, unsigned long long ulEndTime, 
                    std::string sQueueName) {
    TEST_LOG_DETAIL("Start Ana Result: "+ sQueueName + ", ReadCount: " + std::to_string(vecTime.size()));

    if (vecTime.size() == 0) return "";

    unsigned long long ulEndNanosecs = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    unsigned long long ulMin = 0; 
    unsigned long long ulMax = 0;
    unsigned long long ulAve = 0;
    std::sort(vecTime.begin(), vecTime.end());

    ulMin = vecTime[0];
    ulMax = vecTime[vecTime.size()-1];

    unsigned long long ul25 = vecTime[std::floor(vecTime.size()*25/100)];
    unsigned long long ul50 = vecTime[std::floor(vecTime.size()/2)];
    unsigned long long ul75 = vecTime[std::floor(vecTime.size()*75/100)];
    unsigned long long ul90 = vecTime[std::floor(vecTime.size()*9/10)];

    std::string sDelayTimeInfo = "Start: " + NanoToMicroString(ulStartTime) 
                        + ", End: " + NanoToMicroString(ulEndTime)
                        + ", cost: " + NanoStr((ulEndTime - ulStartTime))
                        + ", ave: " + NanoStr((ulEndTime - ulStartTime) / vecTime.size()) + "\n"
                        + "dataCount: " + std::to_string(vecTime.size()) 
                        +  ", min=" + NanoStr(ulMin) + ", max=" + NanoStr(ulMax)
                        + ", 25%=" + NanoStr(ul25) + ", 50%=" + NanoStr(ul50) 
                        + ", 75%=" + NanoStr(ul75) + ", 90%=" + NanoStr(ul90);
    
    std::string sThroughput = "";
    unsigned long long  costNanosecs = ulEndTime - ulStartTime;
    if (costNanosecs > 0) {
        double dBlockSumCounts = vecTime.size();
        unsigned long long dBlockPerSecs = dBlockSumCounts * NANO_PER_SECOND / costNanosecs ;

        int BlockSize = 1;

        if (metaData.iFixedBlock == 1) {
            BlockSize = sizeof(DataBlockFixed);
        } else if (metaData.iFixedBlock == 2) {
            BlockSize = sizeof(unsigned long long );
        }
        unsigned long long dBytesPerSecs = dBlockPerSecs * BlockSize;

        sThroughput +=  ", dBlockPerSecs: " 
                
                + std::to_string(dBlockPerSecs) + ", KB/S: " + std::to_string(dBytesPerSecs/1024) 
                + " KB";      
    }

    std::string strMsg =  "\nPush To Pop Delay Time: \n" + sDelayTimeInfo + "\n" + sThroughput + "\n\n";                                

    return strMsg;
}
