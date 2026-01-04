#pragma once


#include "comm_define.h"

// 行情频率枚举（支持扩展，新增频率只需在此添加）
enum class BarFrequency {
    TICK = 0,        // 最细粒度：Tick数据（逐笔）
    MINUTE_1 = 1,    // 1分钟线
    MINUTE_5 = 5,    // 5分钟线
    HOUR_1 = 60,     // 1小时线
    DAY = 1440,      // 日线（1440分钟）
    WEEK = 10080     // 周线（10080分钟）
};

enum class KlineIndicatorType {
    Alpha_001 = 1,        // 最细粒度：Tick数据（逐笔）
    Alpha_010 = 10,    // 1分钟线
    Alpha_036 = 36
};

struct DepthDataAtom {
    char exchange[3]; // 交易所（SH/SZ）
    char stock_code[10]; // 证券代码
    double bid_price[5]; // 买盘价格
    double bid_volume[5]; // 买盘成交量
    double ask_price[5]; // 卖盘价格
    double ask_volume[5]; // 卖盘成交量
    unsigned long long timestamp; // 时间戳（纳秒）
    unsigned long long ulID;
};

using DepthDataAtomSharedPtr = std::shared_ptr<DepthDataAtom>;

struct DepthData {
    my_vector<my_vector<DepthDataAtom>> depth_data_atoms;   // 深度数据，第一层代表股票代码，第二层代码同一个股票代码，多个时刻的深度数据
    my_vector<my_mutex> depth_mutex_;

};
using DepthDataSharePtr = std::shared_ptr<DepthData>;


inline unsigned int GetDataLimit(BarFrequency iFrequency) {
    switch (iFrequency) {
        case BarFrequency::TICK:
            return 1000000;
        case BarFrequency::MINUTE_1:
            return 1000000;
        case BarFrequency::MINUTE_5:
            return 1000000;
        case BarFrequency::HOUR_1:
            return 1000000;
        case BarFrequency::DAY:
            return 1000000;
        case BarFrequency::WEEK:
            return 1000000;
        default:
            return 1000000;
    }
}

struct KlineIndicator {
    char exchange[3]; // 交易所（SH/SZ）
    char stock_code[10]; // 证券代码
    unsigned long long timestamp; // 时间戳（纳秒）
    int bar_index; // 时间戳对应的K线索引, 1,5,60,1440, 10080;
    unsigned short stock_index; // 股票索引
};
using KlineIndicatorSharedPtr = std::shared_ptr<KlineIndicator>;


struct KlineAtom {
    char exchange[3]; // 交易所（SH/SZ）
    char stock_code[10]; // 证券代码
    double open_price; // 开盘价
    double high_price; // 最高价
    double low_price; // 最低价
    double close_price; // 收盘价
    double volume; // 成交量
    double amount; // 成交额
    unsigned long long timestamp; // 时间戳（纳秒）
    int bar_index; // 时间戳对应的K线索引, 1,5,60,1440, 10080;
    unsigned short stock_index; // 股票索引

    KlineIndicatorSharedPtr pKlineIndicator;  // 存储K线指标;
};
using KlineAtomSharedPtr = std::shared_ptr<KlineAtom>;



struct MarketData {
        char exchange[3]; // 交易所（SH/SZ）
        char stock_code[10]; // 证券代码
        double open; // 开盘价
        double high; // 最高价
        double low; // 最低价
        double close; // 收盘价
        double volume; // 成交量
        unsigned long long timestamp; // 时间戳（纳秒）
        unsigned long long ulID;

    MarketData() : open(11), high(0), low(0), close(0), volume(0), timestamp(0) {
        strcpy(exchange, "SH");
        strcpy(stock_code, "600000");
    }

    MarketData(const MarketData& other):open{other.open}, high{other.high}, low{other.low}, close{other.close}, volume{other.volume}, timestamp{other.timestamp} {
        strcpy(exchange, other.exchange);
        strcpy(stock_code, other.stock_code);
    }

    void SetRandomData() {
        open++;
        high = open + 10;
        low = open - 10;
        close = open + 5;
        volume = 1000000;

        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    std::string str() const {
        return std::string("exchange:") + std::string(exchange) 
                + std::string(", stock_code:") + std::string(stock_code) 
                + std::string(", open:") + std::to_string(open) 
                + std::string(", high:") + std::to_string(high) 
                + std::string(", low:") + std::to_string(low) 
                + std::string(", close:") + std::to_string(close) 
                + std::string(", volume:") + std::to_string(volume) 
                + std::string(", timestamp:") + std::to_string(timestamp);
    }
};

struct IndexData {

    IndexData():ulID{0},dAlpha01{0}, dAlpha10{0}, dAlpha36{0} {

    }

    IndexData(const IndexData& other) {
        ulID = other.ulID;
        dAlpha01 = other.dAlpha01;
        dAlpha10 = other.dAlpha10;
        dAlpha36 = other.dAlpha36;
    }

    unsigned long long ulID;
    double dAlpha01;
    double dAlpha10;
    double dAlpha36;
};

const int ErrSuccess = 0;
const int ErrFuncPointerIsNull = -1;
const int ErrFuncParam1IsNull = -2;

struct Fund {
    double dFund;
};

struct StockHold {
    double dStock;
};

struct OrderReq {
    char exchange[3]; // 交易所（SH/SZ）
    char stock_code[10]; // 证券代码
    char side; // 买卖方向（1：买，2：卖）
    char order_type; // 订单类型（1：限价委托，2：本方最优，3：对手方最优剩余转限价，4：市价立即成交剩余撤销，5：市价全额成交或撤销，6：市价最优五档全额成交剩余撤销）
    double price; // 价格（限价委托必填）
    double volume; // 成交量（必填）
    unsigned long long timestamp; // 时间戳（纳秒）

    OrderReq() : side('1'), order_type('1'), price(0), volume(0), timestamp(0) {
        strcpy(exchange, "SH");
        strcpy(stock_code, "600000");
        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    std::string str() const {
        return std::string("exchange:") + std::string(exchange) 
                + std::string(", stock_code:") + std::string(stock_code) 
                + std::string(", side:") + std::string(1, side) 
                + std::string(", order_type:") + std::string(1, order_type) 
                + std::string(", price:") + std::to_string(price) 
                + std::string(", volume:") + std::to_string(volume) 
                + std::string(", timestamp:") + std::to_string(timestamp);
    }
};

class StrategyProcess;

using MarketDataCallbackFuncType = std::function<void(const MarketData&)>;

using IndexDataCallbackFuncType = std::function<void(const IndexData&)>;

// 对应 dll_class_create
typedef void* (funcStrategyCreateFunc)();
// 对应 dll_class_destroy
typedef void (funcStrategyDestroyFunc)(void* pStrategyImpler);

// 处理行情接口;+
typedef int (funcProcessMarketData)(void* pStrategyImpler, MarketData* pMarketData);

// 处理指标数据接口;+
typedef int (funcProcessIndexData)(void* pStrategyImpler, IndexData* pIndexData);

// 注册应用主函数接口;+
typedef int (funcRegisterAppMain)(void* pStrategyImpler, StrategyProcess* pStrategyProcess);

// ===================== 跨平台显式加载动态库 API 封装 =====================
#if defined(_WIN32) || defined(_WIN64)
    // Windows 平台 API
    #include <windows.h>
    typedef HMODULE DllHandle;          // 动态库句柄类型
    #define DLL_INVALID_HANDLE NULL     // 无效句柄
    // 加载动态库
    static DllHandle dll_load(const char* dll_path) {
        return LoadLibraryA(dll_path);
    }
    // 获取接口地址
    static void* dll_get_proc(DllHandle handle, const char* func_name) {
        return (void*)GetProcAddress(handle, func_name);
    }
    // 卸载动态库
    static void dll_unload(DllHandle handle) {
        if (handle != DLL_INVALID_HANDLE) {
            FreeLibrary(handle);
        }
    }
    // 错误信息获取
    static const char* dll_get_error() {
        static char err_buf[256] = {0};
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, err_buf, sizeof(err_buf), NULL);
        return err_buf;
    }
#else
    // Linux/Mac 平台 API
    #include <dlfcn.h>
    typedef void* DllHandle;            // 动态库句柄类型
    #define DLL_INVALID_HANDLE NULL     // 无效句柄
    // 加载动态库
    static DllHandle dll_load(const char* dll_path) {
        return dlopen(dll_path, RTLD_LAZY);
    }
    // 获取接口地址
    static void* dll_get_proc(DllHandle handle, const char* func_name) {
        return dlsym(handle, func_name);
    }
    // 卸载动态库
    static void dll_unload(DllHandle handle) {
        if (handle != DLL_INVALID_HANDLE) {
            dlclose(handle);
        }
    }
    // 错误信息获取
    static const char* dll_get_error() {
        return dlerror();
    }
#endif

struct TradeUnitDllInfo
{
    TradeUnitDllInfo(const std::string& lib_name) : lib_name_(lib_name) {

    }

    bool LoadDll() {
        DllHandle dll_handle = dll_load(lib_name_.c_str());
        if (dll_handle == DLL_INVALID_HANDLE) {
            std::cerr << "[主程序] 加载动态库失败！错误信息：" << lib_name_ 
                        << " , error: " << dll_get_error() << std::endl;
            return false;
        }
        std::cout << "[主程序] 动态库加载成功！路径：" << lib_name_ << std::endl;

        pFuncStrategyCreate = (funcStrategyCreateFunc*)dll_get_proc(dll_handle, "dll_class_create");
        if (pFuncStrategyCreate == nullptr) {
            std::cerr << "[主程序] 加载动态库失败！错误信息：pFuncStrategyCreate 为空 "  << std::endl;
            return false;
        }
        pFuncStrategyDestroy = (funcStrategyDestroyFunc*)dll_get_proc(dll_handle, "dll_class_destroy");
        if (pFuncStrategyDestroy == nullptr) {
            std::cerr << "[主程序] 加载动态库失败！错误信息：pFuncStrategyDestroy 为空 "  << std::endl;
            return false;
        }
        pFuncProcessMarketData = (funcProcessMarketData*)dll_get_proc(dll_handle, "dll_process_market_data");
        if (pFuncProcessMarketData == nullptr) {
            std::cerr << "[主程序] 加载动态库失败！错误信息：pFuncProcessMarketData 为空 "  << std::endl;
            return false;
        }
        pFuncProcessIndexData = (funcProcessIndexData*)dll_get_proc(dll_handle, "dll_process_index_data");
        if (pFuncProcessIndexData == nullptr) {
            std::cerr << "[主程序] 加载动态库失败！错误信息：pFuncProcessIndexData 为空 "  << std::endl;
            return false;
        }
        pFuncRegisterAppMain = (funcRegisterAppMain*)dll_get_proc(dll_handle, "dll_register_app_main");
        if (pFuncRegisterAppMain == nullptr) {
            std::cerr << "[主程序] 加载动态库失败！错误信息：pFuncRegisterAppMain 为空 "  << std::endl;
            return false;
        }

        pStrategyImpler = pFuncStrategyCreate();
        if (pStrategyImpler == nullptr) {
            std::cerr << "[主程序] 加载动态库失败！错误信息：pStrategyImpler 为空 "  << std::endl;
            return false;
        }

        return true;
    }

    ~TradeUnitDllInfo() {
        if (pFuncStrategyDestroy) {
            pFuncStrategyDestroy(pStrategyImpler);
        }
    }

    int ProcessMarketData(MarketData* pMarketData) {
        if (pFuncProcessMarketData) {
            return pFuncProcessMarketData(pStrategyImpler, pMarketData);
        } else {
            return ErrFuncPointerIsNull;
        }

        return ErrSuccess;
    }

    int ProcessIndexData(IndexData* pIndexData) {
        if (pFuncProcessIndexData) {
            return pFuncProcessIndexData(pStrategyImpler, pIndexData);
        } else {
            return ErrFuncPointerIsNull;
        }

        return ErrSuccess;
    }

    int RegisterAppMain(StrategyProcess* pStrategyProcess) {
        if (pFuncRegisterAppMain) {
            return pFuncRegisterAppMain(pStrategyImpler, pStrategyProcess);
        } else {
            return ErrFuncPointerIsNull;
        }

        return ErrSuccess;
    }

    std::string lib_name_;  

    void* pStrategyImpler;
    funcRegisterAppMain *pFuncRegisterAppMain;
	funcProcessMarketData *pFuncProcessMarketData;
    funcProcessIndexData *pFuncProcessIndexData;
    funcStrategyCreateFunc* pFuncStrategyCreate;
    funcStrategyDestroyFunc* pFuncStrategyDestroy;


};

using TradeUnitDllInfoPtr = std::shared_ptr<TradeUnitDllInfo>;


#ifdef __GNUC__
/**
 * @brief      生成有利于分支预测的代码,告知编译器条件表达式"x"为false的概率较高
 *
 * @param      x     条件表达式
 *
 * @return     条件表达式的布尔值
 */
#define SHARE_COMM_LIKELY(x) (__builtin_expect(!!(x), 1))
#else
#define SHARE_COMM_LIKELY(x) (x)
#endif
