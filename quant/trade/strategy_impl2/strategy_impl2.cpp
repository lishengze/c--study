#include "strategy_dll.h"
#include <iostream>
#include "strategy_process.h"
#include "share_comm_external_message.h"

class StrategyImpl2 : public IStrateImpl {
public:
    int ProcessKlineAtom(KlineAtom* pMarketData) override {
        // std::cout << "StrategyImpl2::ProcessMarketData: " << pMarketData->str() << std::endl;
        logger_->info("StrategyImpl2::ProcessMarketData: {}", pMarketData->str());
        
        if (pStrategyProcess_ == nullptr) {
            return ErrFuncParam1IsNull;
        } else {
            OrderReq order_req(pMarketData->stock_code,2, pMarketData->close_price, pMarketData->volume/2);
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


    StrategyImpl2* pStrategyImpl = (StrategyImpl2*)pStrategyHandler;
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

    StrategyImpl2* pStrategyImpl = (StrategyImpl2*)pStrategyHandler;
    return pStrategyImpl->ProcessIndexData(pIndexData);
}


LIB_API int dll_register_app_main(void* pStrategyHandler, StrategyProcess* pStrategyProcess) {
    if (pStrategyHandler == nullptr ) {
        return ErrFuncPointerIsNull;
    }

    if (pStrategyProcess == nullptr) {
        return ErrFuncParam1IsNull;
    }

    StrategyImpl2* pStrategyImpl = (StrategyImpl2*)pStrategyHandler;
    
    return pStrategyImpl->RegisterAppMain(pStrategyProcess);
}

LIB_API void* dll_class_create() {
    StrategyImpl2* dll_obj = new StrategyImpl2();
    printf("[动态库封装接口] 创建 StrategyImpl2 实例\n");
    return (void*)dll_obj;  // 转换为 void* 隐藏类型
}

LIB_API void dll_class_destroy(void* dll_obj_handle) {
    if (dll_obj_handle != nullptr) {
        StrategyImpl2* dll_obj = (StrategyImpl2*)dll_obj_handle;
        delete dll_obj;
        printf("[动态库封装接口] 销毁 StrategyImpl2 实例\n");
    }
}
