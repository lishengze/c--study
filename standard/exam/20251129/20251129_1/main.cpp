#include <iostream>
#include <thread>
#include <memory>
#include <chrono>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <atomic>
using namespace std;


using threadPtr = std::shared_ptr<std::thread>;
using mutexPtr = std::shared_ptr<std::mutex>;
using conditionVariablePtr = std::shared_ptr<std::condition_variable>;

void SendTradeMessage(string& stock_name) {
    cout << "当前股票为：" << stock_name << endl;
}   

struct StockAtom {
    StockAtom(string& stock_name, mutexPtr stock_mutex_ptr, conditionVariablePtr stock_condition_wait_ptr, conditionVariablePtr stock_condition_notify_ptr) 
    : stock_name_(stock_name), stock_mutex_ptr_(stock_mutex_ptr), stock_condition_wait_ptr_(stock_condition_wait_ptr), stock_condition_notify_ptr_(stock_condition_notify_ptr) {

    }

    string stock_name_;         // 股票名称;
    std::shared_ptr<mutex> stock_mutex_ptr_;         // 股票交易的互斥锁 与 stock_condition_wait_ 配合使用，保护当前股票的交易操作；
    std::shared_ptr<condition_variable> stock_condition_wait_ptr_;    // 唤醒当前股票进行交易的条件变量，按照顺序被当前股票的前一股票在交易完成后唤醒；
    std::shared_ptr<condition_variable> stock_condition_notify_ptr_;    // 当前股票交易完成后，唤醒下一个股票进行交易的条件变量；
};
std::atomic<bool> gIsTradeFinish;

void trade_main(StockAtom& stock_atom, int iTradeMillSec) {

    while(gIsTradeFinish) {
        std::unique_lock<std::mutex> lock(*stock_atom.stock_mutex_ptr_);
        stock_atom.stock_condition_wait_ptr_->wait(lock, [&]() { return true; });  //Once 唤醒当前股票进行交易的条件变量，按照顺序被当前股票的前一股票在交易完成后唤醒；
        SendTradeMessage(stock_atom.stock_name_);
        std::this_thread::sleep_for(std::chrono::milliseconds(iTradeMillSec));
        stock_atom.stock_condition_notify_ptr_->notify_one();
    }

}

/// @brief 工作线程的主函数，负责按照指定的股票序列进行交易；
/// @param vecStockNameList 股票序列，按照顺序进行交易；
/// @param iTradeMillSec 每个股票的交易间隔时间，单位为毫秒；
void work_main(std::vector<string> vecStockNameList, int iTradeMillSec) {
    std::vector<StockAtom> vecStockAtomList;
    std::vector<threadPtr> vecThreadPtrList;
    int index = 0;
    mutexPtr stock_mutex_ptr = std::make_shared<mutex>();
    conditionVariablePtr firstNotifyPtr;

    for (int i = 0; i < vecStockNameList.size(); ++i) {
        std::string stock_name = vecStockNameList[i];

        conditionVariablePtr stock_condition_notify_ptr = std::make_shared<condition_variable>();

        if (i == 0) {
            vecStockAtomList.emplace_back(StockAtom{stock_name, stock_mutex_ptr, nullptr, stock_condition_notify_ptr});   
        } else {
            vecStockAtomList.emplace_back(StockAtom{stock_name, stock_mutex_ptr, vecStockAtomList[i-1].stock_condition_notify_ptr_, stock_condition_notify_ptr});   
        }
             
    }   

    vecStockAtomList[0].stock_condition_wait_ptr_ = vecStockAtomList[vecStockAtomList.size()-1].stock_condition_notify_ptr_;
    firstNotifyPtr = vecStockAtomList[0].stock_condition_wait_ptr_ ;

     for (int i = 0; i < vecStockAtomList.size(); ++i) {
        threadPtr thread_ptr = std::make_shared<std::thread>(trade_main, std::ref(vecStockAtomList[i]), iTradeMillSec);
        vecThreadPtrList.push_back(thread_ptr);
    }

    firstNotifyPtr->notify_one();  // 唤醒第一个股票进行交易；
    gIsTradeFinish.store(true);
     
     for (auto& thread_ptr: vecThreadPtrList) {
        thread_ptr->join();
    }

}

int main() {
    cout << "开始买卖指定的股票序列" << endl;

    work_main({"A", "B", "C"}, 1000);   

    return 0;
}