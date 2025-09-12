#include "struct.h"


DataBlockPtr GetRandomDataBlock() {
    std::random_device rd;  // 用于获取随机种子
    std::mt19937 gen(rd()); // 以随机设备作为种子的 Mersenne Twister 生成器

    // 设置随机数的范围
    std::uniform_int_distribution<> distrib(1, 12); // 1 到 12 之间的均匀分布
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
            return std::make_shared<DataBlock11>();
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
        data_block2->CopyBlock(pBlock);
    }
    return data_block2;    
}

void CopyDataBlockToBuffer(char* pBuffer, DataBlock* pSrcBlockOri) {
    std::cout << "CopyDataBlockToBuffer " << pSrcBlockOri->array_size_ << ", size: " << pSrcBlockOri->size_ << std::endl;
    switch (pSrcBlockOri->array_size_) {
        case 4:{
           std::cout << "Copy 4" << std::endl;
        //    DataBlock* pDstBlock = (DataBlock*)pBuffer;
        //    (* pDstBlock) = (* pSrcBlockOri);
            DataBlock* pDstBlock = new (pBuffer) DataBlock1();
            DataBlock1* pSrcBlock = (DataBlock1*)pSrcBlockOri;
            pDstBlock->CopyBlock(pSrcBlock);

            //  (* pDstBlock) = (* pSrcBlock);
            memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
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
            DataBlock2* pDstBlock = (DataBlock2*)pBuffer;
            DataBlock2* pSrcBlock = (DataBlock2*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
            break;
        }
        case 16:{
            DataBlock3* pDstBlock = (DataBlock3*)pBuffer;
            DataBlock3* pSrcBlock = (DataBlock3*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
            break;
        }
        case 32:{
            DataBlock4* pDstBlock = (DataBlock4*)pBuffer;
            DataBlock4* pSrcBlock = (DataBlock4*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
            break;
        }
        case 64:{
            DataBlock5* pDstBlock = (DataBlock5*)pBuffer;
            DataBlock5* pSrcBlock = (DataBlock5*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
            break;
        }
        case 128:{
            DataBlock6* pDstBlock = (DataBlock6*)pBuffer;
            DataBlock6* pSrcBlock = (DataBlock6*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
            break;
        }
        case 256:{
            DataBlock7* pDstBlock = (DataBlock7*)pBuffer;
            DataBlock7* pSrcBlock = (DataBlock7*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
            break;
        }
        case 512:{
            DataBlock8* pDstBlock = (DataBlock8*)pBuffer;
            DataBlock8* pSrcBlock = (DataBlock8*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
            break;
        }
        case 1024:{
            DataBlock9* pDstBlock = (DataBlock9*)pBuffer;
            DataBlock9* pSrcBlock = (DataBlock9*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
            break;
        }
        case 2048:{
            DataBlock10* pDstBlock = (DataBlock10*)pBuffer;
            DataBlock10* pSrcBlock = (DataBlock10*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->array_size_);
            break;
        }
        case 4096:{
            DataBlock11* pDstBlock = (DataBlock11*)pBuffer;
            DataBlock11* pSrcBlock = (DataBlock11*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);
            pDstBlock->size_ = pSrcBlock->size_;
            
            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->size_);
            break;
        }
        case 8192:{
            DataBlock12* pDstBlock = (DataBlock12*)pBuffer;
            DataBlock12* pSrcBlock = (DataBlock12*)pSrcBlockOri;
            memcpy(pDstBlock->data_, pSrcBlock->data_, pSrcBlock->array_size_);            

            pDstBlock->size_ = pSrcBlock->size_;


            // pDstBlock->CopyBlock(pSrcBlock);
            // memcpy(pDstBlock->GetData(), pSrcBlock->GetData(), pSrcBlock->size_);
            break;
        }
        default:
            std::cout << "Unknonw array_size_: " << pSrcBlockOri->array_size_ << std::endl;
            break;
    }
}

void test_struct() {
    DataBlockPtr data_block = GetRandomDataBlock();
    if (data_block) {
        unsigned char* pdata = data_block->GetData();
        std::cout << "source data_block size: " << data_block->size_ << ", array_size_: " << data_block->array_size_ <<", " << (int)(pdata[data_block->array_size_-1])<< std::endl;
    }

    char* pCData = new char[data_block->size_];
    CopyDataBlockToBuffer(pCData, data_block.get());

    DataBlock* pdata_block = (DataBlock*)pCData;
    // (*pCData) = *(data_block->GetData());

    // unsigned char* pArray = pdata_block->GetData();
    // std::cout << "source data_block size: " << pdata_block->size_ << ", array_size_: " << pdata_block->array_size_ <<", " << (int)(pArray[pdata_block->array_size_-1])<< std::endl;
    

    DataBlockPtr data_block2 = GetCopyBlock(data_block);
    if (data_block2) {
        unsigned char* pdata = data_block2->GetData();
        std::cout << "copied data_block size: " << data_block2->size_ << ", array_size_: " << data_block2->array_size_ <<", " << (int)pdata[data_block2->array_size_-1]<< std::endl;
    }

    delete[] pCData;
}


