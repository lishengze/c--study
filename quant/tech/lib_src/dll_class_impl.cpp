#define LIB_BUILD  // 标记：当前编译动态库，触发 LIB_API = DLL_EXPORT
#include "../include/dll_class_interface.h"
#include <iostream>

// ===================== 动态库类：业务逻辑实现 =====================
// 成员函数：处理自定义结构体数据
int DllBusinessClass::process_data(const MyCustomStruct* in_data, MyCustomStruct* out_data) {
    if (in_data == nullptr || out_data == nullptr) {
        m_status = -1;
        return -1;
    }

    // 业务逻辑：复制入参数据，并修改部分字段作为输出
    memcpy(out_data, in_data, sizeof(MyCustomStruct));
    out_data->id += 100;  // ID 偏移 100
    out_data->score *= 1.5f;  // 分数放大 1.5 倍
    snprintf(out_data->name, sizeof(out_data->name), "%s_Processed", in_data->name);
    out_data->timestamp += 1000;  // 时间戳偏移 1000

    printf("[动态库类] 处理自定义结构体数据：\n");
    printf("  输入 ID: %d, 输出 ID: %d\n", in_data->id, out_data->id);
    printf("  输入 Score: %.2f, 输出 Score: %.2f\n", in_data->score, out_data->score);
    printf("  输入 Name: %s, 输出 Name: %s\n", in_data->name, out_data->name);

    std::cout << "***** 在动态链接库中调用 主类资源 *******" << std::endl;
    m_app_obj->app_business_func("哈哈哈哈哈哈！");

    m_status = 1;  // 标记处理成功
    return 0;
}

// 成员函数：触发主程序回调
void DllBusinessClass::trigger_callback(const MyCustomStruct* callback_data) {
    if (m_app_callback != nullptr && m_app_obj != nullptr && callback_data != nullptr) {
        printf("[动态库类] 准备回调主程序类接口...\n");
        m_app_callback(m_app_obj, callback_data);  // 调用主程序类成员函数（传入 this 指针 + 结构体）
    } else {
        printf("[动态库类] 回调失败：回调函数未注册或参数为空！\n");
    }
}

// ===================== C 风格封装接口：动态库导出（桥梁） =====================
// 创建动态库类实例
LIB_API void* dll_class_create() {
    DllBusinessClass* dll_obj = new DllBusinessClass();
    printf("[动态库封装接口] 创建 DllBusinessClass 实例\n");
    return (void*)dll_obj;  // 转换为 void* 隐藏类型
}

// 销毁动态库类实例
LIB_API void dll_class_destroy(void* dll_obj_handle) {
    if (dll_obj_handle != nullptr) {
        DllBusinessClass* dll_obj = (DllBusinessClass*)dll_obj_handle;
        delete dll_obj;
        printf("[动态库封装接口] 销毁 DllBusinessClass 实例\n");
    }
}

// 封装：调用 DllBusinessClass::process_data
LIB_API int dll_class_process_data(void* dll_obj_handle, const MyCustomStruct* in_data, MyCustomStruct* out_data) {
    if (dll_obj_handle == nullptr) {
        return -1;
    }
    DllBusinessClass* dll_obj = (DllBusinessClass*)dll_obj_handle;
    return dll_obj->process_data(in_data, out_data);
}

// 封装：调用 DllBusinessClass::get_status
LIB_API int dll_class_get_status(void* dll_obj_handle) {
    if (dll_obj_handle == nullptr) {
        return -1;
    }
    DllBusinessClass* dll_obj = (DllBusinessClass*)dll_obj_handle;
    return dll_obj->get_status();
}

// 封装：调用 DllBusinessClass::register_callback
LIB_API void dll_class_register_callback(void* dll_obj_handle, AppClassCallbackFunc callback, AppBusinessClass* app_obj) {
    if (dll_obj_handle == nullptr) {
        return;
    }
    DllBusinessClass* dll_obj = (DllBusinessClass*)dll_obj_handle;
    dll_obj->register_callback(callback, app_obj);
}

// 封装：调用 DllBusinessClass::trigger_callback
LIB_API void dll_class_trigger_callback(void* dll_obj_handle, const MyCustomStruct* callback_data) {
    if (dll_obj_handle == nullptr) {
        return;
    }
    DllBusinessClass* dll_obj = (DllBusinessClass*)dll_obj_handle;
    dll_obj->trigger_callback(callback_data);
}