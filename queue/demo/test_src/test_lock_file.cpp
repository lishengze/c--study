#include "test_lock_file.h"
#include "logger.h"
#include "lock_file_manager.h"
#include <iostream>

using namespace std;

// using share_common::LockFileManager;

using namespace share_common;

/// @brief 测试获取策略批次号的功能;
void test_GetSetStrategyBatchID(){
    LOG_INFO("[START]: test_GetSetStrategyBatchID ------------ \n");

    // LOG_TRACE("test_GetSetStrategyBatchID start");

    LockFileManager lockFileManager;

    unsigned int uiStrategySysID = 1000000;

    unsigned int uiBatchID1 = lockFileManager.GetSetStrategyBatchID(uiStrategySysID);

    unsigned uiBatchID2 = lockFileManager.GetSetStrategyBatchID(uiStrategySysID);

    if (uiBatchID1 != uiBatchID2 - 1) {
        LOG_ERROR("uiBatchID1!= uiBatchID2 - 1,  uiBatchID1: {}, uiBatchID2: {}", uiBatchID1, uiBatchID2);

        // std::cerr << "[ERROR]: uiBatchID1!= uiBatchID2 - 1,  uiBatchID1: " << uiBatchID1 << ", uiBatchID2: " << uiBatchID2 << std::endl;
    } else {
        LOG_INFO("uiBatchID1 == uiBatchID2 - 1, uiBatchID1: {}, uiBatchID2: {}", uiBatchID1, uiBatchID2);

        // std::cout << "[SUCCESS]: uiBatchID1 == uiBatchID2 - 1, uiBatchID1: " << uiBatchID1 << ", uiBatchID2: " << uiBatchID2 << std::endl;
    }

    LOG_INFO("[END]: test_GetSetStrategyBatchID ------------ \n");
}

void test_strategy_lock_file() {

}

void test_lock_file() {
    test_GetSetStrategyBatchID();
}