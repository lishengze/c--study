#include <cstring>
#include <string>

#include <unordered_map>
#include <iostream>
#include <vector>
#include <mutex>
#include <set>

#include <array>
#include <functional>
#include <algorithm>
#include <string>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <set>

#include <cmath>
#include <stdexcept>
#include <numeric>

#include "spdlog/spdlog.h"
#include "spdlog/async_logger.h"
#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"



// 模板化通用别名，可复用
template <typename T>
using my_vector = std::vector<T>;

template <typename T>
using my_set = std::set<T>;

template <typename key, typename value>
using my_unorder_map = std::unordered_map<key, value>;

using my_mutex = std::mutex;

using my_string = std::string;

using spdlog_ptr = std::shared_ptr<spdlog::logger>;
