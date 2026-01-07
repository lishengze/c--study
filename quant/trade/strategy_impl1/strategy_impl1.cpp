#include "strategy_dll.h"
#include <iostream>
#include "strategy_process.h"
#include "share_comm_external_message.h"

class StrategyImpl1 : public IStrateImpl {
public:
    int ProcessKlineAtom(KlineAtom* pMarketData) override {
        // std::cout << "StrategyImpl1::ProcessMarketData: " << pMarketData->str() << std::endl;
        logger_->info("StrategyImpl1::ProcessMarketData: {}", pMarketData->str(pConfigManager_->GetIndicatorSet()));
        
        if (pStrategyProcess_ == nullptr) {
            return ErrFuncParam1IsNull;
        } else {
            OrderReq order_req(pMarketData->stock_code, 1, pMarketData->close_price, pMarketData->volume/2);
            pStrategyProcess_->SendOrderReq(order_req);
        }
        
        return ErrSuccess;
    }

    int ProcessIndexData(IndexData* pIndexData) override {
        return ErrSuccess;
    }
};

// 动态库类成员函数封装：业务处理（传入自定义结构体）
LIB_API int dll_process_market_data(void* pStrategyHandler,  KlineAtom* pMarketData) {
    if (pStrategyHandler == nullptr ) {
        return ErrFuncPointerIsNull;
    }

    if (pMarketData == nullptr) {
        return ErrFuncParam1IsNull;
    }


    StrategyImpl1* pStrategyImpl = (StrategyImpl1*)pStrategyHandler;
    return pStrategyImpl->ProcessKlineAtom(pMarketData);
}

// 动态库类成员函数封装：获取类状态
LIB_API int dll_process_index_data(void* pStrategyHandler, IndexData* pIndexData) {
    if (pStrategyHandler == nullptr ) {
        return ErrFuncPointerIsNull;
    }

    if (pIndexData == nullptr) {
        return ErrFuncParam1IsNull;
    }

    StrategyImpl1* pStrategyImpl = (StrategyImpl1*)pStrategyHandler;
    return pStrategyImpl->ProcessIndexData(pIndexData);
}


LIB_API int dll_register_app_main(void* pStrategyHandler, StrategyProcess* pStrategyProcess) {
    if (pStrategyHandler == nullptr ) {
        return ErrFuncPointerIsNull;
    }

    if (pStrategyProcess == nullptr) {
        return ErrFuncParam1IsNull;
    }

    StrategyImpl1* pStrategyImpl = (StrategyImpl1*)pStrategyHandler;
    
    return pStrategyImpl->RegisterAppMain(pStrategyProcess);
}

LIB_API void* dll_class_create() {
    StrategyImpl1* dll_obj = new StrategyImpl1();
    printf("[动态库封装接口] 创建 StrategyImpl1 实例\n");
    return (void*)dll_obj;  // 转换为 void* 隐藏类型
}

LIB_API void dll_class_destroy(void* dll_obj_handle) {
    if (dll_obj_handle != nullptr) {
        StrategyImpl1* dll_obj = (StrategyImpl1*)dll_obj_handle;
        delete dll_obj;
        printf("[动态库封装接口] 销毁 StrategyImpl1 实例\n");
    }
}
