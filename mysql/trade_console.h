///作者: 李晟泽
///创建时间: 2021-04-13
///简介：用于所有交易相关数据的交互

#pragma once

#include "pandora/package/package_station.h"
#include "pandora/util/singleton.hpp"
#include "pandora/util/thread_basepool.h"
#include "pandora/messager/ut_log.h"
#include "pandora/util/time_util.h"

#include "quark/cxx/ut/UtData.h"
#include "DBEngine.h"


class TradeConsole: public utrade::pandora::ThreadBasePool, public IPackageStation
{
public:
    TradeConsole(utrade::pandora::io_service_pool& engine_pool, IPackageStation* next_station);
    virtual ~TradeConsole(){}

    void launch();
    void release();

    void request_message(PackagePtr package);
    void response_message(PackagePtr package);

    void handle_request_message(PackagePtr package);
    void handle_response_message(PackagePtr package);

    void req_rollback_info(PackagePtr package);

    void req_create_order(PackagePtr package);
    void req_cancel_order(PackagePtr package);

    void rsp_create_order(PackagePtr package);
    void rsp_cancel_order(PackagePtr package);

    void rtn_order(PackagePtr package);
    void rtn_trade(PackagePtr package);


private:

    DBEnginePtr     db_engine_{nullptr};
};

DECLARE_PTR(TradeConsole);
