#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

// ===================== 1. 跨平台导出/导入宏定义 =====================
#if defined(_WIN32) || defined(_WIN64)
    #define DLL_EXPORT __declspec(dllexport)
    #define DLL_IMPORT __declspec(dllimport)
#else
    #define DLL_EXPORT extern "C"
    #define DLL_IMPORT extern "C"
#endif

// ===================== 2. 自定义数据结构（POD 类型，保证内存布局一致） =====================
// 自定义结构体：入参/返回值使用，必须保证主程序与动态库定义完全一致
typedef struct {
    int32_t id;                // 整数ID
    char name[64];             // 字符数组（避免 std::string，提升兼容性）
    float score;               // 浮点值
    uint64_t timestamp;        // 时间戳
} MyCustomStruct;              // 简化版：POD 类型，无复杂成员

// ===================== 3. 前向声明（隐藏实现细节） =====================
// 动态库中的类（前向声明，主程序不感知实现）
class DllBusinessClass;
// 主程序中的类（前向声明，动态库不感知实现）
class AppBusinessClass;

// ===================== 4. 回调函数类型定义（适配类成员函数，封装 this 指针） =====================
// 回调函数原型：传递 主程序类实例指针(this) + 自定义结构体
typedef void (*AppClassCallbackFunc)(AppBusinessClass* app_obj, const MyCustomStruct* struct_data);

// ===================== 5. C 风格封装接口（动态库导出，作为交互桥梁） =====================
// 标记：动态库编译时定义 LIB_BUILD，主程序编译时不定义
#ifdef LIB_BUILD
    #define LIB_API DLL_EXPORT
#else
    #define LIB_API DLL_IMPORT
#endif

// -------------------- 动态库类实例管理接口 --------------------
// 创建动态库类实例（返回实例句柄，隐藏具体类型）
LIB_API void* dll_class_create();
// 销毁动态库类实例（释放内存）
LIB_API void dll_class_destroy(void* dll_obj_handle);

// -------------------- 主程序 → 动态库：调用动态库类成员函数（入参为自定义结构体） --------------------
// 动态库类成员函数封装：业务处理（传入自定义结构体）
LIB_API int dll_class_process_data(void* dll_obj_handle, const MyCustomStruct* in_data, MyCustomStruct* out_data);
// 动态库类成员函数封装：获取类状态
LIB_API int dll_class_get_status(void* dll_obj_handle);

// -------------------- 动态库 → 主程序：注册回调接口（回调主程序类成员函数） --------------------
// 注册主程序类的回调函数（传入 回调函数指针 + 主程序类实例指针）
LIB_API void dll_class_register_callback(void* dll_obj_handle, AppClassCallbackFunc callback, AppBusinessClass* app_obj);
// 触发回调：动态库主动调用主程序类的回调接口（传入自定义结构体）
LIB_API void dll_class_trigger_callback(void* dll_obj_handle, const MyCustomStruct* callback_data);

// ===================== 6. 主程序类声明（供动态库回调） =====================
class AppBusinessClass {
public:
    // 主程序类成员函数（供动态库回调，入参为自定义结构体）
    void on_dll_callback(const MyCustomStruct* struct_data) {
        if (struct_data == nullptr) {
            printf("[主程序类] 回调数据为空！\n");
            return;
        }
        printf("[主程序类] 收到动态库回调，结构体数据：\n");
        printf("  ID: %d\n", struct_data->id);
        printf("  Name: %s\n", struct_data->name);
        printf("  Score: %.2f\n", struct_data->score);
        printf("  Timestamp: %lu\n", struct_data->timestamp);
    }

    // 主程序类业务方法
    void app_business_func(std::string msg) {
        printf("[主程序类] 执行自身业务逻辑 %s\n",msg.c_str());
    }
};

// ===================== 7. 动态库类声明（供主程序调用） =====================
class DllBusinessClass {
public:
    // 动态库类成员函数（核心业务接口，入参/出参为自定义结构体）
    int process_data(const MyCustomStruct* in_data, MyCustomStruct* out_data);
    // 动态库类成员函数（获取状态）
    int get_status() const { return m_status; }
    // 注册主程序回调
    void register_callback(AppClassCallbackFunc callback, AppBusinessClass* app_obj) {
        m_app_callback = callback;
        m_app_obj = app_obj;
        printf("[动态库类] 主程序回调接口注册成功！\n");
    }
    // 触发主程序回调
    void trigger_callback(const MyCustomStruct* callback_data);

private:
    int m_status = 0;  // 类状态变量
    AppClassCallbackFunc m_app_callback = nullptr;  // 主程序回调函数指针
    AppBusinessClass* m_app_obj = nullptr;          // 主程序类实例指针
};