#include "../include/dll_class_interface.h"
#include <iostream>

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

int import_from_compile() {
    std::cout << "===== 主程序启动 =====" << std::endl;

    // --------------- 1. 创建主程序类实例 ---------------
    AppBusinessClass app_obj;
    app_obj.app_business_func();

    // --------------- 2. 创建动态库类实例（通过 C 风格封装接口） ---------------
    void* dll_obj = dll_class_create();
    if (dll_obj == nullptr) {
        std::cerr << "[主程序] 创建动态库类实例失败！" << std::endl;
        return -1;
    }

    // --------------- 3. 注册主程序类回调接口给动态库 ---------------
    // 回调函数封装：适配 C 风格函数指针（传递 this 指针）
    auto app_callback = [](AppBusinessClass* app_obj, const MyCustomStruct* struct_data) {
        app_obj->on_dll_callback(struct_data);  // 调用主程序类成员函数
    };
    dll_class_register_callback(dll_obj, app_callback, &app_obj);

    // --------------- 4. 主程序调用动态库类成员函数（传入自定义结构体） ---------------
    MyCustomStruct in_data, out_data;
    // 初始化输入结构体
    init_custom_struct(&in_data, 10, "TestStruct", 85.5f, 1735689600);
    // 调用动态库接口处理数据
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

    // --------------- 5. 获取动态库类状态 ---------------
    int dll_status = dll_class_get_status(dll_obj);
    std::cout << "\n[主程序] 动态库类当前状态：" << dll_status << std::endl;

    // --------------- 6. 触发动态库回调主程序类接口 ---------------
    MyCustomStruct callback_data;
    init_custom_struct(&callback_data, 20, "CallbackStruct", 92.0f, 1735693200);
    dll_class_trigger_callback(dll_obj, &callback_data);

    // --------------- 7. 销毁动态库类实例 ---------------
    dll_class_destroy(dll_obj);
    std::cout << "\n===== 主程序退出 =====" << std::endl;

    return 0;
}

