#pragma once

#include "comm_define.h"
#include "indicator.h"
#include "share_comm_util.h"
#include "config_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#pragma pack(push, 1)

// // 生成 [min, max] 整数随机数
// int rand_int(int min, int max) {
//     srand((unsigned int)time(NULL));

//     // 校验参数合法性
//     if (min > max) {
//         int temp = min;
//         min = max;
//         max = temp;
//     }
//     return min + rand() % (max - min + 1);
// }

// 生成 [min, max) 浮点数随机数
inline double rand_float(double min, double max) {
    if (min >= max) return min;
    return min + (double)rand() / RAND_MAX * (max - min);
}





// 行情频率枚举（支持扩展，新增频率只需在此添加）
enum class BarFrequency {
    TICK = 0,        // 最细粒度：Tick数据（逐笔）
    MINUTE_1 = 1,    // 1分钟线
    MINUTE_5 = 5,    // 5分钟线
    HOUR_1 = 60,     // 1小时线
    DAY = 1440,      // 日线（1440分钟）
    WEEK = 10080     // 周线（10080分钟）
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
            return 100000;
        case BarFrequency::MINUTE_1:
            return 12000;
        case BarFrequency::MINUTE_5:
            return 2400;
        case BarFrequency::HOUR_1:
            return 2400;
        case BarFrequency::DAY:
            return 2400;
        case BarFrequency::WEEK:
            return 2400;
        default:
            return 100000;
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
    double high_price; // 最高价q
    double low_price; // 最低价
    double close_price; // 收盘价
    double volume; // 成交量
    double amount; // 成交额
    unsigned long long timestamp; // 时间戳（纳秒）
    int bar_index; // 时间戳对应的K线索引, 1,5,60,1440, 10080;
    unsigned short stock_index; // 股票索引

    double alpha_1;
    double alpha_10;
    double alpha_36;

    // IndicatorAtom indicator_atom;

    KlineAtom() {

    }

    KlineAtom(const KlineAtom& other) {
        if (this == &other) return;
        memcpy(exchange, other.exchange, 3);
        memcpy(stock_code, other.stock_code, 10);
        open_price = other.open_price;
        high_price = other.high_price;
        low_price = other.low_price;
        close_price = other.close_price;
        volume = other.volume;
        amount = other.amount;
        timestamp = other.timestamp;
        bar_index = other.bar_index;
        stock_index = other.stock_index;
        alpha_1 = other.alpha_1;
        alpha_10 = other.alpha_10;
        alpha_36 = other.alpha_36;
    }

    KlineAtom(const my_string& stock_code, const int stock_index, const int bar_index) : open_price(11), high_price(0), low_price(0), close_price(0), volume(0), timestamp(0) {
        strcpy(this->stock_code, stock_code.c_str());
        this->stock_index = stock_index;
        this->bar_index = bar_index;
    }

    void UpdateKlineIndicatorValue(KlineIndicatorType indicator_type, double value) {
        // indicator_atom.UpdateIndicatorValue(indicator_type, value);

        switch (indicator_type) {
            case KlineIndicatorType::Alpha_001:
                alpha_1 = value;
                break;
            case KlineIndicatorType::Alpha_010:
                alpha_10 = value;
                break;
            case KlineIndicatorType::Alpha_036:
                alpha_36 = value;
                break;
            default:
                break;
        }

    }

    void SetRandomData() {
        open_price = rand_float(5, 10);
        high_price = rand_float(10,15);
        low_price = rand_float(1, 5);
        close_price = rand_float(5, 10);
        volume = rand_float(100,1000);
        amount = close_price * volume;

        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    // my_string str() const {
    //     return my_string("exchange:") + my_string(exchange) 
    //             + my_string(", stock_code:") + my_string(stock_code) 
    //             + my_string(", time:") + ToSecondStr(timestamp)
    //             + my_string(", bar_index:") + std::to_string(bar_index)
    //             + my_string(", stock_index:") + std::to_string(stock_index)                
    //             + my_string(", \nO:") + std::to_string(open_price)
    //             + my_string(", H:") + std::to_string(high_price)
    //             + my_string(", L:") + std::to_string(low_price)
    //             + my_string(", C:") + std::to_string(close_price)
    //             + my_string(", V:") + std::to_string(volume)
    //             + my_string(", A:") + std::to_string(amount)                
 
    //             + my_string(", alpha_1:") + std::to_string(alpha_1)
    //             + my_string(", alpha_10:") + std::to_string(alpha_10)
    //             + my_string(", alpha_36:") + std::to_string(alpha_36)
    //             ;       
    // }

    my_string str() const {
        my_string ret = my_string(stock_code) + my_string(", O:") + std::to_string(open_price)
                + my_string(", H:") + std::to_string(high_price)
                + my_string(", L:") + std::to_string(low_price)
                + my_string(", C:") + std::to_string(close_price)
                + my_string(", V:") + std::to_string(volume)
                + my_string(", A:") + std::to_string(amount);
        
        my_set<int> vecIndicatorTypes = CONFIG_MANAGER_INSTANCE->GetIndicatorSet();

        for (auto indicatorType : vecIndicatorTypes) {
            switch (indicatorType) {
                case int(KlineIndicatorType::Alpha_001):
                    ret += my_string(", alpha_1:") + std::to_string(alpha_1);
                    break;
                case int(KlineIndicatorType::Alpha_010):
                    ret += my_string(", alpha_10:") + std::to_string(alpha_10);
                    break;
                case int(KlineIndicatorType::Alpha_036):
                    ret += my_string(", alpha_36:") + std::to_string(alpha_36);
                    break;
                default:
                    break;
            }
        }

        return  ret;       
    } 
    
    my_string str(my_set<int>& vecIndicatorTypes ) const {
        my_string ret = my_string(stock_code) + my_string(", O:") + std::to_string(open_price)
                + my_string(", H:") + std::to_string(high_price)
                + my_string(", L:") + std::to_string(low_price)
                + my_string(", C:") + std::to_string(close_price)
                + my_string(", V:") + std::to_string(volume)
                + my_string(", A:") + std::to_string(amount);
        
        for (auto indicatorType : vecIndicatorTypes) {
            switch (indicatorType) {
                case int(KlineIndicatorType::Alpha_001):
                    ret += my_string(", alpha_1:") + std::to_string(alpha_1);
                    break;
                case int(KlineIndicatorType::Alpha_010):
                    ret += my_string(", alpha_10:") + std::to_string(alpha_10);
                    break;
                case int(KlineIndicatorType::Alpha_036):
                    ret += my_string(", alpha_36:") + std::to_string(alpha_36);
                    break;
                default:
                    break;
            }
        }

        return  ret;       
    }     
};
using KlineAtomSharedPtr = std::shared_ptr<KlineAtom>;

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
    unsigned int uiStrategyID;

    OrderReq() : side('1'), order_type('1'), price(0), volume(0), timestamp(0) {
        strcpy(exchange, "SH");
        strcpy(stock_code, "600000");
        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    OrderReq(char* stock_code_src, unsigned int uiStrategyIDSrc,  double price, double volume) : 
    side('1'), order_type('1'), price(price), volume(volume), timestamp(0), uiStrategyID(uiStrategyIDSrc) {
        strcpy(stock_code, stock_code_src);
        timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }    

    my_string str() const {
        return  my_string("StaID:") + std::to_string(uiStrategyID) 
                + my_string(", stock_code:") + my_string(stock_code) 
                + my_string(", side:") + my_string(1, side) 
                + my_string(", price:") + std::to_string(price) 
                + my_string(", volume:") + std::to_string(volume) 
                + my_string(", timestamp:") + NanoToMicroString(timestamp)
                ;
    }
};

class StrategyProcess;

// 对应 dll_class_create
typedef void* (funcStrategyCreateFunc)();
// 对应 dll_class_destroy
typedef void (funcStrategyDestroyFunc)(void* pStrategyImpler);

// 处理行情接口;+
typedef int (funcProcessMarketData)(void* pStrategyImpler, KlineAtom* pMarketData);

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

class StrategyProcess;

class IStrateImpl {
public:
    IStrateImpl():logger_{nullptr}, pStrategyProcess_{nullptr} {
        
    }
    virtual int ProcessKlineAtom(KlineAtom* pMarketData) = 0;

    virtual int ProcessIndexData(IndexData* pIndexData) = 0;
    
    virtual int RegisterAppMain(StrategyProcess* pStrategyProcess)  {
        pStrategyProcess_ = pStrategyProcess;
        return ErrSuccess;
    }

    virtual int RegisterConfigManager(ConfigManager * pConfigManager)  {
        pConfigManager_ = pConfigManager;
        return ErrSuccess;
    }    

    void SetLogger(spdlog_ptr logger) {
        logger_ = logger;
    }

protected:
    spdlog_ptr logger_;    
    StrategyProcess* pStrategyProcess_;
    ConfigManager* pConfigManager_;
};


struct TradeUnitDllInfo
{
    TradeUnitDllInfo(const my_string& lib_name, const my_string lib_path=".") : lib_name_(lib_name), lib_path_(lib_path) {
        
    }

    bool LoadDll() {
        my_string full_lib_path = lib_path_ + "/lib" + lib_name_ + ".so";
        DllHandle dll_handle = dll_load(full_lib_path.c_str());
        if (dll_handle == DLL_INVALID_HANDLE) {
            std::cerr << "[主程序] 加载动态库失败！错误信息：" << full_lib_path 
                        << " , error: " << dll_get_error() << std::endl;
            return false;
        }
        LOG_INFO("LoadDll, full_lib_path: {}, SUCCESS", full_lib_path);

        // std::cout << "[主程序] 动态库加载成功！路径：" << full_lib_path << std::endl;

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

        if (!InitLogger()) {
            return false;
        }

        IStrateImpl* tmp = (IStrateImpl*)(pStrategyImpler);

        if (tmp == nullptr) {
            std::cerr << "[主程序] 加载动态库失败！错误信息：tmp 为空 "  << std::endl;
            return false;
        }

        pConfigManager_ = CONFIG_MANAGER_INSTANCE.get();

        tmp->SetLogger(logger_);
        tmp->RegisterConfigManager(pConfigManager_);

        return true;
    }

    bool InitLogger() {
        // logger_ = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>(
        //     lib_name_+".log", 
        //     lib_name_, 
        //     (std::size_t)1024 * 1024 * 1024 * 3, 
        //     1000); 

        // logger_ = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>(
        //     lib_name_+".log", 
        //     true, 
        //     "w"); 

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(lib_name_+".log", true);
        logger_ = std::make_shared<spdlog::logger>(lib_name_, file_sink);            

        if (logger_ == nullptr) {
            std::cerr << "[主程序] 初始化日志失败！错误信息：logger_ 为空 "  << std::endl;
            return false;
        }

        logger_->set_pattern("[%H:%M:%S],%t,[%l],%s,[%!],%#|%v");
        logger_->set_level(spdlog::level::debug);
        logger_->flush_on(spdlog::level::err);
        spdlog::flush_every(std::chrono::seconds(1));
        logger_->set_level(spdlog::level::debug);
        return true;
    }

    ~TradeUnitDllInfo() {
        if (pFuncStrategyDestroy) {
            pFuncStrategyDestroy(pStrategyImpler);
        }

        if (logger_) {
            logger_->flush();
            spdlog::drop_all();
        }
    }

    int ProcessKlineAtom(KlineAtom* pKlineAtom) {
        if (pFuncProcessMarketData) {
            return pFuncProcessMarketData(pStrategyImpler, pKlineAtom);
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

    my_string lib_name_;  
    my_string lib_path_;  


    void* pStrategyImpler;
    funcRegisterAppMain *pFuncRegisterAppMain;
	funcProcessMarketData *pFuncProcessMarketData;
    funcProcessIndexData *pFuncProcessIndexData;
    funcStrategyCreateFunc* pFuncStrategyCreate;
    funcStrategyDestroyFunc* pFuncStrategyDestroy;

    spdlog_ptr logger_;
    ConfigManager* pConfigManager_;
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


using KlineAtomCallbackFuncType = std::function<void(const KlineAtom&)>;

using KlineVectorCallbackFuncType = std::function<void(const std::vector<KlineAtomSharedPtr>&)>;

using IndexDataCallbackFuncType = std::function<void(const IndexData&)>;

#pragma pack(pop)