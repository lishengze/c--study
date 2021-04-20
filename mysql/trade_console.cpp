#include "trade_console.h"

#include "quark/cxx/ut/UtPackageDesc.h"
#include "quark/cxx/assign.h"
#include "pandora/util/time_util.h"
#include "quark/cxx/CopyEntity.h"
#include "quark/cxx/Utils.h"
#include "quark/cxx/ut/UtPrintUtils.h"

#include <boost/make_shared.hpp>
#include <algorithm>
using std::sort;


TradeConsole::TradeConsole(utrade::pandora::io_service_pool& pool, IPackageStation* next_station)
    : ThreadBasePool(pool)
    , IPackageStation(next_station)
{

}

void TradeConsole::launch()
{
    try
    {
        string host = "127.0.0.1:3306";
        string usr = "wormhole";
        string pwd = "worm";
        string schema = "TestTrade";
        int    port = 3306;

        db_engine_ = boost::make_shared<DBEngine>(usr, pwd, schema, port, host);

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void TradeConsole::release()
{
    try
    {
        /* code */
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void TradeConsole::request_message(PackagePtr package)
{
    std::cout << "TradeConsole::request_message  " << package->Tid() << std::endl;
    get_io_service().post(std::bind(&TradeConsole::handle_request_message, this, package));
}

void TradeConsole::response_message(PackagePtr package)
{
    get_io_service().post(std::bind(&TradeConsole::handle_response_message, this, package));
}

void TradeConsole::handle_request_message(PackagePtr package)
{
    std::cout << "TradeConsole::handle_request_message " << package->Tid() << std::endl;
    try
    {
        switch (package->Tid())
        {
        case UT_TID_ReqCreateOrder:
            req_create_order(package);
            break;

        case UT_TID_ReqCancelOrder:
            req_cancel_order(package);
            break;

        case UT_TID_ReqRollbackInfo:
            req_rollback_info(package);
            break;        
        
        default:
            break;
        }

        deliver_request(package);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void TradeConsole::handle_response_message(PackagePtr package)
{
    try
    {
        switch (package->Tid())
        {
        case UT_TID_RspCreateOrder:
            rsp_create_order(package);
            break;

        case UT_TID_RtnOrder:
            rtn_order(package);
            break;

        case UT_TID_RtnTrade:
            rtn_trade(package);
            break;        
        
        case UT_TID_RspCancelOrder:
            rsp_cancel_order(package);
            break;

        default:
            break;
        }

        deliver_response(package);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

unsigned long get_package_time(PackagePtr package)
{
    try
    {
        unsigned long result = 0;

        switch (package->Tid())
        {
            case UT_TID_RspReqCreateOrder:
            {
                auto* p = GET_FIELD(package, CUTRspReqCreateOrderField);
                if (!p)
                {
                    std::cout << "ReqCreateOrder Data is NULL! " << endl;
                    break;
                }
                else
                {
                    result = p->SendTime;
                }
                /* code */
                break;
            }
            case UT_TID_RspReqCancelOrder:
            {
                auto* p = GET_FIELD(package, CUTRspReqCancelOrderField);
                if (!p)
                {
                    std::cout << "ReqCancelOrder Data is NULL! " << endl;
                    break;
                }
                else
                {
                    result = p->SendTime;
                }
                /* code */
                break;
            }

            case UT_TID_RtnOrder:
            {
                auto* p = GET_FIELD(package, CUTRtnOrderField);
                if (!p)
                {
                    std::cout << "RtnOrder Data is NULL! " << endl;
                    break;
                }
                else
                {
                    result = p->RspLocalTime;                }
                /* code */
                break;
            }

            case UT_TID_RtnTrade:
            {
                auto* p = GET_FIELD(package, CUTRtnTradeField);
                if (!p)
                {
                    std::cout << "RtnTrade Data is NULL! " << endl;
                    break;
                }
                else
                {
                    result = p->RspLocalTime;
                }
                /* code */
                break;
            }
            case UT_TID_RspCreateOrder:
            {
                auto* p = GET_FIELD(package, CUTRspCreateOrderField);
                if (!p)
                {
                    std::cout << "RspCreateOrder Data is NULL! " << endl;
                    break;
                }
                else
                {
                    result = p->RspLocalTime;
                }
                /* code */
                break;
            }
            case UT_TID_RspCancelOrder:
            {
                auto* p = GET_FIELD(package, CUTRspCancelOrderField);
                if (!p)
                {
                    std::cout << "RspCancelOrder Data is NULL! " << endl;
                    break;
                }
                else
                {
                    result = p->RspLocalTime;
                }
                /* code */
                break; 
            }       
        default:
            cout << "Unkown Package TID: " << package->Tid() << endl;
            break;
        }
        return result;
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] get_package_time " <<  e.what() << '\n';
    }
}

struct {
    bool operator() (PackagePtr a, PackagePtr b) const 
    { 
        return get_package_time(a) < get_package_time(b); 
    }
} TimeCmp;

void TradeConsole::req_rollback_info(PackagePtr package)
{
    try
    {
        std::cout << "TradeConsole::req_rollback_info " << std::endl;
        /* code */

        const CUTReqRollbackInfoField* pReqRollbakInfo = GET_FIELD(package, CUTReqRollbackInfoField);

        if (!pReqRollbakInfo)
        {
            cout << "pReqRollbakInfo is null " << endl;
            return;
        }
        else
        {
            printUTData(pReqRollbakInfo, UT_FID_ReqRollbackInfo);
        }

        string account_name = pReqRollbakInfo->AccountName;
        unsigned long start_time = pReqRollbakInfo->TimeStart;
        unsigned long end_time = pReqRollbakInfo->TimeEnd;



        if (!db_engine_)
        {
            cout << "DataBase was not connected!" << endl;
        }
        else
        {
            std::vector<PackagePtr> package_list;
            std::vector<string> create_order_local_id_list;

            std::vector<PackagePtr> req_create_order_package_list = db_engine_->get_req_create_order_by_time(account_name, start_time, end_time);

            for (PackagePtr req_create_order_package: req_create_order_package_list)
            {
                if (req_create_order_package)
                {                                        
                    auto* pRspReqCreateOrder = GET_FIELD(req_create_order_package, CUTRspReqCreateOrderField);
                    if (pRspReqCreateOrder)
                    {
                        string order_loca_id = pRspReqCreateOrder->OrderLocalID;
                        string session_id = pRspReqCreateOrder->SessionID;

                        cout << "req_create_order_local_id: " << order_loca_id << endl;

                        PackagePtr rtn_order_package = db_engine_->get_rtn_order_by_orderlocalid(account_name, order_loca_id);

                        PackagePtr rtn_trade_package = db_engine_->get_rtn_trade_by_orderlocalid(account_name, order_loca_id);

                        PackagePtr rsp_create_order_package = db_engine_->get_rsp_create_order_by_orderlocalid(account_name, order_loca_id);

                        req_create_order_package->prepare_response(UT_TID_RspReqCreateOrder, 0, CHAIN_LAST, session_id);
                        package_list.push_back(req_create_order_package);

                        rtn_order_package->prepare_response(UT_TID_RtnOrder, 0, CHAIN_LAST, session_id);
                        package_list.push_back(rtn_order_package);
                        
                        rtn_trade_package->prepare_response(UT_TID_RtnTrade, 0, CHAIN_LAST, session_id);
                        package_list.push_back(rtn_trade_package);

                        rsp_create_order_package->prepare_response(UT_TID_RspCreateOrder, 0, CHAIN_LAST, session_id);
                        package_list.push_back(rsp_create_order_package);    
                    }
                }
            }

            // for(PackagePtr package:create_order_package_list)
            // {
            //     auto* p = GET_FIELD(package, CUTRspReqCreateOrderField);
            //     cout << endl;
            //     printUTData(p, UT_FID_RspReqCreateOrder);
            // }

            std::vector<string> cancel_order_local_id_list;

            std::vector<PackagePtr> req_cancel_order_package_list =  db_engine_->get_req_cancel_order_by_time(account_name, start_time, end_time);

            for (PackagePtr rspreq_cancel_order_package: req_cancel_order_package_list)
            {
                auto* pRspReqCancelOrder = GET_FIELD(rspreq_cancel_order_package, CUTRspReqCancelOrderField);

                if (pRspReqCancelOrder)
                {
                    string order_loca_id = pRspReqCancelOrder->OrderLocalID;
                    string session_id = pRspReqCancelOrder->SessionID;
                    cout << "req_cancel_order_local_id: " << order_loca_id << endl;

                    PackagePtr rsp_cancel_order_package = db_engine_->get_rsp_cancel_order_by_orderlocalid(account_name, order_loca_id);

                    rspreq_cancel_order_package->prepare_response(UT_TID_RspReqCancelOrder, 0, CHAIN_LAST, session_id);
                    package_list.push_back(rspreq_cancel_order_package);

                    rsp_cancel_order_package->prepare_response(UT_TID_RspCancelOrder, 0, CHAIN_LAST, session_id);
                    package_list.push_back(rsp_cancel_order_package);
                }

            }

            sort(package_list.begin(), package_list.end(), TimeCmp);

            for (auto p:package_list)
            {
                cout << std::setw(40) << GetTIDInfo(p->Tid()) << " " << utrade::pandora::ToSecondStr(get_package_time(p)) << endl;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] TradeConsole::req_rollback_info " << e.what() << '\n';
    }

}

void TradeConsole::req_create_order(PackagePtr package)
{
    try
    {
        if (package)
        {
            const CUTReqCreateOrderField* p = GET_FIELD(package, CUTReqCreateOrderField);

            if (db_engine_)
            {
                db_engine_->insert_req_create_order(*p);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] TradeConsole::req_create_order " << e.what() << '\n';
    }
}

void TradeConsole::req_cancel_order(PackagePtr package)
{
    try
    {
        if (package)
        {
            const CUTReqCancelOrderField* p = GET_FIELD(package, CUTReqCancelOrderField);

            if (db_engine_)
            {
                db_engine_->insert_req_cancel_order(*p);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] TradeConsole::req_cancel_order " << e.what() << '\n';
    }
}

void TradeConsole::rsp_create_order(PackagePtr package)
{
    try
    {
        if (package)
        {
            const CUTRspCreateOrderField* pRspCreateOrder = GET_FIELD(package, CUTRspCreateOrderField);
            const CUTRspInfoField* pRspInfo = GET_FIELD(package, CUTRspInfoField);

            if (!pRspCreateOrder)
            {
                cout << "TradeConsole::rsp_create_order pRspCreateOrder is NULL" << endl;
                return;
            }

            if (!pRspInfo)
            {
                cout << "TradeConsole::rsp_create_order pRspInfo is NULL" << endl;
                return;
            }            

            if (db_engine_)
            {
                db_engine_->insert_rsp_create_order(*pRspCreateOrder, *pRspInfo);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] TradeConsole::rsp_create_order " << e.what() << '\n';
    }
}

void TradeConsole::rsp_cancel_order(PackagePtr package)
{
    try
    {
        if (package)
        {
            const CUTRspCancelOrderField* pRspCancelOrder = GET_FIELD(package, CUTRspCancelOrderField);
            const CUTRspInfoField* pRspInfo = GET_FIELD(package, CUTRspInfoField);

            if (!pRspCancelOrder)
            {
                cout << "TradeConsole::rsp_cancel_order pRspCancelOrder is NULL" << endl;
                return;
            }

            if (!pRspInfo)
            {
                cout << "TradeConsole::rsp_cancel_order pRspInfo is NULL" << endl;
                return;
            }            

            if (db_engine_)
            {
                db_engine_->insert_rsp_cancel_order(*pRspCancelOrder, *pRspInfo);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] TradeConsole::rsp_cancel_order " << e.what() << '\n';
    }
}

void TradeConsole::rtn_order(PackagePtr package)
{
    try
    {
        if (package)
        {
            const CUTRtnOrderField* pRtnOrder = GET_FIELD(package, CUTRtnOrderField);
            const CUTRspInfoField* pRspInfo = GET_FIELD(package, CUTRspInfoField);

            if (!pRtnOrder)
            {
                cout << "TradeConsole::rtn_order pRtnOrder is NULL" << endl;
                return;
            }

            if (!pRspInfo)
            {
                cout << "TradeConsole::rtn_order pRspInfo is NULL" << endl;
                return;
            }            

            if (db_engine_)
            {
                db_engine_->insert_rtn_order(*pRtnOrder, *pRspInfo);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] TradeConsole::rtn_order " << e.what() << '\n';
    }
}

void TradeConsole::rtn_trade(PackagePtr package)
{
    try
    {
        if (package)
        {
            const CUTRtnTradeField* pRtnTrade = GET_FIELD(package, CUTRtnTradeField);
            const CUTRspInfoField* pRspInfo = GET_FIELD(package, CUTRspInfoField);

            if (!pRtnTrade)
            {
                cout << "TradeConsole::rtn_trade pRtnTrade is NULL" << endl;
                return;
            }

            if (!pRspInfo)
            {
                cout << "TradeConsole::rtn_trade pRspInfo is NULL" << endl;
                return;
            }            

            if (db_engine_)
            {
                db_engine_->insert_rtn_trade(*pRtnTrade, *pRspInfo);
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\n[E] TradeConsole::rtn_trade " << e.what() << '\n';
    }
}