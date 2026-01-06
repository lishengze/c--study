#include "market_receiver.h"
#include "config_manager.h"
#include "logger.h"
#include "share_comm_external_message.h"


using namespace share_common;

bool MarketReceiver::Init() {

    if (!InitSrcKlineAtomQueue()) {
        LOG_ERROR("InitSrcKlineAtomQueue Failed");
        return false;
    }

    return true;
}

void MarketReceiver::SendKlineAtomToQueue(const KlineAtom& market_data) {
    LOG_INFO("SendKlineAtomToQueue:\nmarket_data:{}", market_data.str());

    if (ptr_src_market_data_queue_ != nullptr) {

        if (!ptr_src_market_data_queue_->trypush(market_data)) {
            LOG_INFO("SendKlineAtomToQueue, ptr_src_market_data_queue_ try_push failed");
        } else {
            LOG_INFO("SendKlineAtomToQueue, ptr_src_market_data_queue_ try_push success");
        }
    } else {
        LOG_ERROR("SendKlineAtomToQueue, ptr_src_market_data_queue_ is null");
    }
}

bool MarketReceiver::InitSrcKlineAtomQueue() {
    LOG_INFO("InitSrcKlineAtomQueue Start");

    ptr_src_market_data_queue_ = new mpmc_queue<KlineAtom>();

    if (!ptr_src_market_data_queue_->create(4096)) {
        LOG_ERROR("queue create  failed");
        return false;
    }

    KlineAtom market_data;

    // for (int i = 0; i < 10; ++i) {
    //     market_data.SetRandomData();

    //     if (!ptr_src_market_data_queue_->trypush(market_data)) {
    //         LOG_ERROR("InitSrcKlineAtomQueue, ptr_src_market_data_queue_ try_push failed");
    //         return false;
    //     } else {
    //         LOG_INFO("InitSrcKlineAtomQueue, ptr_src_market_data_queue_ try_push success");
    //     }
    // }

                
    return true;
}



bool MarketReceiver::Start() {


    if (CONFIG_MANAGER_INSTANCE->GetStringValue("WorkMode", "Mode", "Test") == "Test") {
        ptr_thread_ = std::make_shared<std::thread>([this]() {
            // 接收线程逻辑
            while (true) {
                // 从源市场行情数据队列中获取数据
                KlineAtom market_data;
                market_data.SetRandomData();
                SendKlineAtomToQueue(market_data);
                std::this_thread::sleep_for(std::chrono::seconds(
                                            CONFIG_MANAGER_INSTANCE->GetIntValue("WorkMode", "DataFreqSec", 5)));                
        }
        });

    } else if (CONFIG_MANAGER_INSTANCE->GetStringValue("WorkMode", "Mode", "Test") == "TestKline") {

        ptr_thread_ = std::make_shared<std::thread>([this]() {
        
        my_vector<KlineAtomSharedPtr> vecKlineAtoms;
        my_unorder_map<my_string, int>& stock_index_dic = CONFIG_MANAGER_INSTANCE->GetStockIndexDic();

        for (auto& iter : stock_index_dic) {
                KlineAtomSharedPtr ptrKlineAtom(new KlineAtom(iter.first, iter.second, 1));
                ptrKlineAtom->SetRandomData();
                vecKlineAtoms.push_back(ptrKlineAtom);
            }


            while (true) {
                // 从源市场行情数据队列中获取数据
                for (auto& ptrKlineAtom : vecKlineAtoms) {
                    ptrKlineAtom->SetRandomData();
                    ptrKlineAtom->bar_index = 1;
                }

                kline_vector_callback_func_(vecKlineAtoms);
                
                std::this_thread::sleep_for(std::chrono::seconds(
                                                CONFIG_MANAGER_INSTANCE->GetIntValue("WorkMode", "DataFreqSec", 5)));                
            }

         });
    } else {
        LOG_INFO("MarketReceiver Start Failed, WorkMode is not Test");
        return true;
    }

    LOG_INFO("MarketReceiver Start Success");

    return true;
}

bool MarketReceiver::Stop() {
    return true;
}