#include "../include/dll_class_interface.h"
#include <iostream>
#include <cstdlib>

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

// ===================== 动态库接口函数指针类型定义（必须与导出接口一致） =====================
// 对应 dll_class_create
typedef void* (*DllClassCreateFunc)();
// 对应 dll_class_destroy
typedef void (*DllClassDestroyFunc)(void* dll_obj_handle);
// 对应 dll_class_process_data
typedef int (*DllClassProcessDataFunc)(void* dll_obj_handle, const MyCustomStruct* in_data, MyCustomStruct* out_data);
// 对应 dll_class_get_status
typedef int (*DllClassGetStatusFunc)(void* dll_obj_handle);
// 对应 dll_class_register_callback
typedef void (*DllClassRegisterCallbackFunc)(void* dll_obj_handle, AppClassCallbackFunc callback, AppBusinessClass* app_obj);
// 对应 dll_class_trigger_callback
typedef void (*DllClassTriggerCallbackFunc)(void* dll_obj_handle, const MyCustomStruct* callback_data);

// 辅助函数：初始化自定义结构体
void init_custom_struct(MyCustomStruct* data, int32_t id, const char* name, float score, uint64_t timestamp) {
    if (data == nullptr || name == nullptr) {
        return;
    }
    data->id = id;
    snprintf(data->name, sizeof(data->name), "%s", name);
    data->score = score;
    data->timestamp = timestamp;
}

int main() {
    std::cout << "===== 主程序启动（显式加载动态库） =====" << std::endl;

    // --------------- 1. 定义动态库路径（跨平台适配） ---------------
    const char* dll_path = nullptr;
    #if defined(_WIN32) || defined(_WIN64)
        dll_path = "class_struct_dll.dll";  // Windows 动态库路径（与EXE同目录）
    #else
        dll_path = "./libapp_dll.so";  // Linux 动态库路径（当前目录）
    #endif

    // --------------- 2. 显式加载动态库 ---------------
    DllHandle dll_handle = dll_load(dll_path);
    if (dll_handle == DLL_INVALID_HANDLE) {
        std::cerr << "[主程序] 加载动态库失败！错误信息：" << dll_get_error() << std::endl;
        return -1;
    }
    std::cout << "[主程序] 动态库加载成功！路径：" << dll_path << std::endl;

    // --------------- 3. 显式获取动态库所有接口地址（函数指针赋值） ---------------
    // 获取 dll_class_create 接口
    DllClassCreateFunc dll_class_create = (DllClassCreateFunc)dll_get_proc(dll_handle, "dll_class_create");
    // 获取 dll_class_destroy 接口
    DllClassDestroyFunc dll_class_destroy = (DllClassDestroyFunc)dll_get_proc(dll_handle, "dll_class_destroy");
    // 获取 dll_class_process_data 接口
    DllClassProcessDataFunc dll_class_process_data = (DllClassProcessDataFunc)dll_get_proc(dll_handle, "dll_class_process_data");
    // 获取 dll_class_get_status 接口
    DllClassGetStatusFunc dll_class_get_status = (DllClassGetStatusFunc)dll_get_proc(dll_handle, "dll_class_get_status");
    // 获取 dll_class_register_callback 接口
    DllClassRegisterCallbackFunc dll_class_register_callback = (DllClassRegisterCallbackFunc)dll_get_proc(dll_handle, "dll_class_register_callback");
    // 获取 dll_class_trigger_callback 接口
    DllClassTriggerCallbackFunc dll_class_trigger_callback = (DllClassTriggerCallbackFunc)dll_get_proc(dll_handle, "dll_class_trigger_callback");

    // 检查所有接口是否获取成功
    if (!dll_class_create || !dll_class_destroy || !dll_class_process_data ||
        !dll_class_get_status || !dll_class_register_callback || !dll_class_trigger_callback) {
        std::cerr << "[主程序] 获取动态库接口失败！错误信息：" << dll_get_error() << std::endl;
        dll_unload(dll_handle);  // 加载失败，卸载动态库
        return -1;
    }
    std::cout << "[主程序] 所有动态库接口获取成功！" << std::endl;

    // --------------- 4. 后续逻辑与之前一致（仅通过函数指针调用接口） ---------------
    // 创建主程序类实例
    AppBusinessClass app_obj;
    app_obj.app_business_func("或活活活活活活 --- 来自于主程序");

    // 创建动态库类实例（通过函数指针调用）
    void* dll_obj = dll_class_create();
    if (dll_obj == nullptr) {
        std::cerr << "[主程序] 创建动态库类实例失败！" << std::endl;
        dll_unload(dll_handle);
        return -1;
    }

    // 注册主程序类回调接口给动态库
    auto app_callback = [](AppBusinessClass* app_obj, const MyCustomStruct* struct_data) {
        app_obj->on_dll_callback(struct_data);
    };
    dll_class_register_callback(dll_obj, app_callback, &app_obj);

    // 主程序调用动态库类成员函数（传入自定义结构体）
    MyCustomStruct in_data, out_data;
    init_custom_struct(&in_data, 10, "TestStruct", 85.5f, 1735689600);
    int ret = dll_class_process_data(dll_obj, &in_data, &out_data);
    if (ret == 0) {
        std::cout << "\n[主程序] 调用动态库接口成功，输出结构体数据：" << std::endl;
        std::cout << "  ID: " << out_data.id << std::endl;
        std::cout << "  Name: " << out_data.name << std::endl;
        std::cout << "  Score: " << out_data.score << std::endl;
        std::cout << "  Timestamp: " << out_data.timestamp << std::endl;
    } else {
        std::cerr << "[主程序] 调用动态库接口失败，返回码：" << ret << std::endl;
    }

    // 获取动态库类状态
    int dll_status = dll_class_get_status(dll_obj);
    std::cout << "\n[主程序] 动态库类当前状态：" << dll_status << std::endl;

    // 触发动态库回调主程序类接口
    MyCustomStruct callback_data;
    init_custom_struct(&callback_data, 20, "CallbackStruct", 92.0f, 1735693200);
    dll_class_trigger_callback(dll_obj, &callback_data);

    // 销毁动态库类实例
    dll_class_destroy(dll_obj);

    // --------------- 5. 显式卸载动态库 ---------------
    dll_unload(dll_handle);
    std::cout << "[主程序] 动态库卸载成功！" << std::endl;
    std::cout << "\n===== 主程序退出 =====" << std::endl;

    return 0;
}