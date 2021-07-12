#include "../include/global_declare.h"
#include "test.h"

#define CPPHTTPLIB_ZLIB_SUPPORT
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "httplib.h"

#include "pandora/util/json.hpp"



#define CA_CERT_FILE "./ca-bundle.crt"

using namespace std;

void test_simple_http_client()
{
// #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
//   httplib::SSLClient cli("localhost", 8080);
//   // httplib::SSLClient cli("google.com");
//   // httplib::SSLClient cli("www.youtube.com");
//   cli.set_ca_cert_path(CA_CERT_FILE);
//   cli.enable_server_certificate_verification(true);
// #else
//   httplib::Client cli("localhost", 8080);
// #endif

//   if (auto res = cli.Get("/hi")) {
//     cout << res->status << endl;
//     cout << res->get_header_value("Content-Type") << endl;
//     cout << res->body << endl;
//   } else {
//     cout << "error code: " << res.error() << std::endl;
// #ifdef CPPHTTPLIB_OPENSSL_SUPPORT
//     auto result = cli.get_openssl_verify_result();
//     if (result) {
//       cout << "verify error: " << X509_verify_cert_error_string(result) << endl;
//     }
// #endif
//   }
}


void test_b2c2()
{
    // #define CPPHTTPLIB_OPENSSL_SUPPORT

    string uri = "https://api.uat.b2c2.net";
    httplib::Client cli(uri.c_str());

    cli.enable_server_certificate_verification(false);
    
    httplib::Headers header;
    string token = "eabe0596c453786c0ecee81978140fad58daf881";
    header.emplace("Authorization", string("Token ") + token);
    header.emplace("Content-Type", "application/json");
    header.emplace("Accept-Encoding", "gzip, deflate");
    header.emplace("Accept", "*/*");
    header.emplace("User-Agent", "python-requests/2.22.0");
    header.emplace("Connection", "keep-alive");

    cli.set_default_headers(header);
    
    nlohmann::json request;
    request["client_order_id"] = "11";
    request["quantity"] = 10;
    request["side"] = "buy";
    request["instrument"] = "XRP.SPOT";
    request["order_type"] = "FOK";
    request["price"] = 0.64;

    if (auto res = cli.Post("/order", header, request.dump(), "application/json"))
    {
      cout << res->status << endl;
      httplib::Headers& headers = res->headers;
      for(auto iter:headers)
      {
        cout << iter.first << ": " << iter.second << endl;
      }
      cout <<"body: " << res->body << endl;
    }
    else
    {
      cout << "error code: " << res.error() << std::endl;
    }
}

void TestMain()
{
    test_b2c2();

    // test_simple_http_client();
}