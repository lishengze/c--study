#pragma once

#include <memory>
#include <random>
#include <chrono>

struct DataBlock {
    unsigned long long start_time_; // 数据块开始时间
    unsigned int  size_; // 数据块大小
};

struct DataBlock1:public DataBlock {
    DataBlock1() {
        size_ = 4;
    }
    unsigned char data_[4]; // 数据块
};

struct DataBlock2:public DataBlock {
    DataBlock2() {
        size_ = 8;
    }
    unsigned char data_[8]; // 数据块
};

struct DataBlock3:public DataBlock {
    DataBlock3() {
        size_ = 16;
    }
    unsigned char data_[16]; // 数据块
};

struct DataBlock4:public DataBlock {
    DataBlock4() {
        size_ = 32;
    }
    unsigned char data_[32]; // 数据块
};

struct DataBlock5:public DataBlock {
    DataBlock5() {
        size_ = 64;
    }
    unsigned char data_[64]; // 数据块
};

struct DataBlock6:public DataBlock {
    DataBlock6() {
        size_ = 128;
    }
    unsigned char data_[128]; // 数据块
};

struct DataBlock7:public DataBlock {
    DataBlock7() {
        size_ = 256;
    }
    unsigned char data_[256]; // 数据块
};

struct DataBlock8:public DataBlock {
    DataBlock8() {
        size_ = 512;
    }
    unsigned char data_[512]; // 数据块
};

struct DataBlock9:public DataBlock {
    DataBlock9() {
        size_ = 1024;
    }
    unsigned char data_[1024]; // 数据块
};

struct DataBlock10:public DataBlock {
    DataBlock10() {
        size_ = 2048;
    }
    unsigned char data_[2048]; // 数据块
};

struct DataBlock11:public DataBlock {
    DataBlock11() {
        size_ = 4096;
    }
    unsigned char data_[4096]; // 数据块
};

struct DataBlock12:public DataBlock {
    DataBlock12() {
        size_ = 8192;
    }
    unsigned char data_[8192]; // 数据块
};


using DataBlockPtr = std::shared_ptr<DataBlock>; 

inline DataBlockPtr GetRandomDataBlock() {
    std::random_device rd;  // 用于获取随机种子
    std::mt19937 gen(rd()); // 以随机设备作为种子的 Mersenne Twister 生成器

    // 设置随机数的范围
    std::uniform_int_distribution<> distrib(1, 12); // 1 到 12 之间的均匀分布
    // 生成随机数
    int random_number = distrib(gen);
    int32_t size = rand() % 8192 + 4;
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
            return std::make_shared<DataBlock11>();
        case 12:
            data_block = std::make_shared<DataBlock12>();
            data_block->size_ = sizeof(DataBlock12);
            break;
        default:
            return nullptr;
    }

    data_block->start_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return data_block;
}