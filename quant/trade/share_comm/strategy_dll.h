#pragma once

#include "share_comm_external_message.h"

#include "comm_define.h"

// ===================== 1. 跨平台导出/导入宏定义 =====================
#if defined(_WIN32) || defined(_WIN64)
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
#else
    #define DLL_EXPORT extern "C"
    #define DLL_IMPORT extern "C"
#endif

// ===================== 5. C 风格封装接口（动态库导出，作为交互桥梁） =====================
// 标记：动态库编译时定义 LIB_BUILD，主程序编译时不定义
#ifdef LIB_BUILD
    #define LIB_API DLL_EXPORT
#else
    #define LIB_API DLL_IMPORT
#endif


// -------------------- 主程序 → 动态库：调用动态库类成员函数（入参为自定义结构体） --------------------
// 动态库类成员函数封装：业务处理（传入自定义结构体）
LIB_API int dll_process_market_data(void* pStrategyHandler,  KlineAtom* pMarketData);

// 动态库类成员函数封装：获取类状态
LIB_API int dll_process_index_data(void* pStrategyHandler, IndexData* pMarketData);


LIB_API int dll_register_app_main(void* pStrategyHandler, StrategyProcess* pStrategyProcess);

LIB_API void* dll_class_create();
// 销毁动态库类实例
LIB_API void dll_class_destroy(void* dll_obj_handle);