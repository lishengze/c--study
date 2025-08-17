#include "test.h"
#include "global_declare.h"
#include "proto/cpp/market_data.pb.h"

#include "json.hpp"

using google::protobuf::RepeatedPtrField;
using njson = nlohmann::json;

void test_base()
{
    Proto3::MarketData::DepthQuote depth_quote;
    // const google::protobuf::RepeatedPtrField<Proto3::MarketData::Depth>& asks = depth_quote.asks();

    Proto3::MarketData::Depth new_depth;
    Proto3::MarketData::Decimal new_price;
    Proto3::MarketData::Decimal new_volume;
    new_price.set_precise(4);
    new_price.set_value(1000);
    new_volume.set_precise(4);
    new_volume.set_value(100000);
    // new_depth.set_allocated_price(&new_price);
    // new_depth.set_allocated_volume(&new_volume);

    Proto3::MarketData::Decimal* mutable_price = new_depth.mutable_price();
    Proto3::MarketData::Decimal* mutable_volume = new_depth.mutable_volume();
    mutable_price->set_precise(4);
    mutable_price->set_value(1000);

    mutable_volume->set_precise(4);
    mutable_volume->set_value(1000);

    google::protobuf::RepeatedPtrField<Proto3::MarketData::Depth>* asks = depth_quote.mutable_asks();

    asks->Add(std::move(new_depth));

    string se_string = depth_quote.SerializeAsString();

    cout << "se_string: "<< se_string << ", size: " << depth_quote.ByteSizeLong() << endl;


     Proto3::MarketData::DepthQuote new_quote;
     new_quote.ParseFromString(se_string);
     cout << new_quote.asks(0).price().precise() << ", " << new_quote.asks(0).price().value() << endl;
}

void test_json()
{
    njson json;
    // json[0] = 4;
    // json[1] = 100;
    // json[2] = 4;
    // json[3] = 10000;

    json["volume"] = 4;
    json["price"] = 100;
    // json[2] = 4;
    // json[3] = 10000;

    string js_string = json.dump();
    cout << "js_string: "<< js_string << ", size: " << js_string.size() << endl;

}

void TestMain()
{
    test_base();

    test_json();
}