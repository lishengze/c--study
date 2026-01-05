#include "trade_manager.h"
#include "config_manager.h"
#include "share_comm_mpmc_queue.h"
#include "logger.h"
#include "share_comm_util.h"

#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <utility>
#include "share_comm_mpmc_queue.h"
#include "share_comm_external_message.h"
#include "bits.h"


using namespace share_common;
using std::shared_ptr;

TradeManager::TradeManager() {

}

bool TradeManager::Init() {

    CONFIG_MANAGER_INSTANCE->Init();

    if (!msg_receiver_.Init()) return false;

    if (!trade_output_.Init()) return false;

    if (!strategy_process_.Init()) return false;

    KlineAtomCallbackFuncType market_data_callback_func = std::bind(&StrategyProcess::OnMarketSrcData, &strategy_process_, std::placeholders::_1);
    msg_receiver_.SetKlineAtomCallbackFunc(market_data_callback_func);


    return true;
}

bool TradeManager::Start() { 

    if (!msg_receiver_.Start()) return false;

    if (!strategy_process_.Start()) return false;    

    if (!trade_output_.Start()) return false;


    return true;

}

