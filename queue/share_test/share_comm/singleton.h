#pragma once

namespace common
{
    enum SingletonType 
    {
        kFund = 0,
        kAccountShare,
        kCashAuctionParam,
        kBizParam,
        kOrderManager,
        kUplinkBiz, 
        kDataLoad, 
        kConfig,
        kTGWManager,
        kSimplifiedFeeInfo,
        kOfferWay,
        kForbiddenInfo,
        kQuantRiskCtrlManager,
        kAGWOrderHelper,
        kFTECmnSecurityInfoManager,
        kCreditDataManager,
        kCreditBizParam,
        kCreditScaleCheckerManager,
        kFTESingleStkTotalScaleGrcManager,
        kFTEPledgeStkWaterlineManager,
        kFTEGRCBizParamManager,
        kFTECreditStkScaleManager,
        kFTECreditPledgeStkSumQtyManager,
        kFTEShareMessageManager,
        kNum
    };
    enum ObjPoolType
    {
        kOrder,
        kObjPoolCount
    };
} // namespace common

#define SINGLETON_DEF(cls_name)   \
     cls_name * test;

#define SINGLETON_IMPL(cls_name, cls_type)   \
    cls_name * tmp;                                  


inline void signleton() {}