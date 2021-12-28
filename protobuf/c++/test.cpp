#include "test.h"
#include "global_declare.h"
#include "proto/cpp/market_data.pb.h"

using google::protobuf::RepeatedPtrField;

void test_base()
{
    Proto3::MarketData::DepthQuote depth_quote;
    const google::protobuf::RepeatedPtrField<Proto3::MarketData::Depth>& asks = depth_quote.asks();

    Proto3::MarketData::Depth* add_asks = depth_quote.add_asks();

    Proto3::MarketData::Depth new_depth;
    Proto3::MarketData::Decimal new_price;
    Proto3::MarketData::Decimal new_volume;
    new_price.set_precise(4);
    new_price.set_value(1000);
    new_volume.set_precise(4);
    new_volume.set_value(100000);
    new_depth.set_allocated_price(&new_price);
    new_depth.set_allocated_volume(&new_volume);

    depth_quote.asks();
}

void TestMain()
{
    test_base();
}