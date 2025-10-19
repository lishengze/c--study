#pragma once

#include "test_ute.h"
#include "test_strategy.h"
#include "test_external_msg.h"
#include "test_lock_file.h"
#include "test_queue.h"


inline void TestMain() {
    test_ute();
    test_strategy();
    test_external_msg();
    test_lock_file();
    test_queue();
}