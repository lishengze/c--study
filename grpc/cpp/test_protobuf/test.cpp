#include "test.h"
#include "global_declare.h"
#include "proto/cpp/test.pb.h"


void test_base()
{
    TestPackage::TestRequest request;
    request.set_name("lsz");
    request.set_time("lsz");
    request.set_session_id("lsz");

    request.set_obj_id("lsz");

    request.set_rpc_id("lsz");

    request.set_request_id("lsz");

    request.set_message("lsz");

    string rst;

    request.SerializeToString(&rst);

    cout << "se.size: " << rst.length() <<", se.rst: " << rst << endl;

}

void TestMain()
{
    test_base();
}