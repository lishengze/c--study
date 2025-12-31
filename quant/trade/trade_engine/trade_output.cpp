#include "trade_output.h"

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

#include "logger.h"

using namespace share_common;
using std::shared_ptr;

bool TradeOutput::Init() {

    return true;
}

bool TradeOutput::Start() {
    return true;
}

bool TradeOutput::Stop() {
    return true;
}
