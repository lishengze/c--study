#include "test_curl.h"

#include <iostream>
#include <string>

#include <curl/curl.h>
#include <curl/easy.h>

#include <stdlib.h>
#include <string.h>
using namespace std;

struct MemoryStruct 
{
    char *memory;
    size_t size;
    MemoryStruct()
    {
        memory = (char *)malloc(1);  
        size = 0;
    }
    ~MemoryStruct()
    {
        free(memory);
        memory = NULL;
        size = 0;
    }
};

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    

    

    cout << "ori_data: " << (char*)(ptr) << "\n"
        << "size: " << size << "\n"
        << "nmemb: " << nmemb << endl;

    // struct MemoryStruct *mem = (struct MemoryStruct *)data;
    // mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);   
    // if (mem->memory) 
    // {
    //     memcpy(&(mem->memory[mem->size]), ptr, realsize);
    //     mem->size += realsize;
    //     mem->memory[mem->size] = 0;
    // }
    return realsize;
}

int test_post_create_order()
{
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if(CURLE_OK != res)
    {
        cout<<"curl init failed"<<endl;
        return 1;
    }

    CURL *pCurl ;
    pCurl = curl_easy_init();

    if( NULL == pCurl)
    {
        cout<<"Init CURL failed..."<<endl;
        return -1;
    }

    string url = "https://api.uat.b2c2.net";
    string filename = "result.json";

    curl_slist *pList = NULL;
    pList = curl_slist_append(pList,"Content-Type:application/json; charset=UTF-8"); 
    // pList = curl_slist_append(pList,"Accept:application/json, */*; q=0.01"); 
    pList = curl_slist_append(pList,"Authorization:Token eabe0596c453786c0ecee81978140fad58daf881"); 
    // pList = curl_slist_append(pList,"Accept-Language:zh-CN,zh;q=0.8"); 
    // curl_easy_setopt(pCurl, CURLOPT_SSH_AUTH_TYPES, pList); 

    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pList); 

    curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str() ); //提交表单的URL地址

    curl_easy_setopt(pCurl, CURLOPT_HEADER, 0L);  //启用时会将头文件的信息作为数据流输
    // curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);//允许重定向
    curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L);

    //将返回结果通过回调函数写到自定义的对象中
    MemoryStruct oDataChunk;
    curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &oDataChunk);
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback); 

    curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L); //启用时会汇报所有的信息
    //post表单参数
    string strJsonData;
    strJsonData = "client_order_id=11&";
    strJsonData += "quantity=10&" ;
    strJsonData += "side=buy&" ;
    strJsonData += "instrument=XRP.SOPT&" ;
    strJsonData += "order_type=FOK&" ;
    strJsonData += "price=0.64&" ;
    strJsonData += "valid_until=2021-07-19T03:41:01&" ;
    strJsonData += "executing_unit=risk-adding-strategy" ;

    //libcur的相关POST配置项
    curl_easy_setopt(pCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strJsonData.c_str());  
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, strJsonData.size());


    res = curl_easy_perform(pCurl);

    long res_code=0;
    res=curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &res_code);

    if(( res == CURLE_OK ) && (res_code == 200 || res_code == 201))
    {
        // FILE* fTmpMem = (FILE*)oDataChunk.memory;
        // if (!fTmpMem) {

        // }
        // FILE *fp=fopen(filename.c_str(),"wb");
        // if(!fp)
        // {   
        //     cout<<"open file failed";
        //     return -1;
        // }   

        // fwrite(fTmpMem, 1, oDataChunk.size, fp);
        // fclose(fp);
        return true;
    }
    else
    {
        cout << "Failed Code: " << res_code << endl;
    }

    curl_slist_free_all(pList); 
    curl_easy_cleanup(pCurl);
    curl_global_cleanup();    
}


// int main()
// {
//     CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
//     if(CURLE_OK != res)
//     {
//         cout<<"curl init failed"<<endl;
//         return 1;
//     }

//     CURL *pCurl ;
//     pCurl = curl_easy_init();

//     if( NULL == pCurl)
//     {
//         cout<<"Init CURL failed..."<<endl;
//         return -1;
//     }

//     string url = "http://{IP}:{PORT}/search";
//     string filename = "result.json";

//     curl_slist *pList = NULL;
//     pList = curl_slist_append(pList,"Content-Type:application/x-www-form-urlencoded; charset=UTF-8"); 
//     pList = curl_slist_append(pList,"Accept:application/json, text/javascript, */*; q=0.01"); 
//     pList = curl_slist_append(pList,"Accept-Language:zh-CN,zh;q=0.8"); 
//     curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pList); 

//     curl_easy_setopt(pCurl, CURLOPT_URL, url.c_str() ); //提交表单的URL地址

//     curl_easy_setopt(pCurl, CURLOPT_HEADER, 0L);  //启用时会将头文件的信息作为数据流输
//     curl_easy_setopt(pCurl, CURLOPT_FOLLOWLOCATION, 1L);//允许重定向
//     curl_easy_setopt(pCurl, CURLOPT_NOSIGNAL, 1L);

//     //将返回结果通过回调函数写到自定义的对象中
//     MemoryStruct oDataChunk;
//     curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &oDataChunk);
//     curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback); 

//     curl_easy_setopt(pCurl, CURLOPT_VERBOSE, 1L); //启用时会汇报所有的信息
//     //post表单参数
//     string strJsonData;
//     strJsonData = "queryWord=CONTENT:码农&";
//     strJsonData += "startTime=1507605327&" ;
//     strJsonData += "endTime=1508210127&" ;
//     strJsonData += "maxCount=500&" ;
//     strJsonData += "contextLength=200" ;
//     //libcur的相关POST配置项
//     curl_easy_setopt(pCurl, CURLOPT_POST, 1L);
//     curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strJsonData.c_str());  
//     curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, strJsonData.size());


//     res = curl_easy_perform(pCurl);

//     long res_code=0;
//     res=curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &res_code);

//     if(( res == CURLE_OK ) && (res_code == 200 || res_code == 201))
//     {
//         FILE* fTmpMem = (FILE*)oDataChunk.memory;
//         if (!fTmpMem) {

//         }
//         FILE *fp=fopen(filename.c_str(),"wb");
//         if(!fp)
//         {   
//             cout<<"open file failed";
//             return -1;
//         }   

//         fwrite(fTmpMem, 1, oDataChunk.size, fp);
//         fclose(fp);
//         return true;
//     }

//     curl_slist_free_all(pList); 
//     curl_easy_cleanup(pCurl);
//     curl_global_cleanup();

//     return 0;
// }
// ————————————————
// 版权声明：本文为CSDN博主「cjf_wei」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
// 原文链接：https://blog.csdn.net/cjf_wei/article/details/79185310

#include <iostream>
#include <ostream>
#include <istream>
#include <stdlib.h>
#include <string.h>
 
#include "curl/curl.h"
#include <stdio.h>
#include <openssl/pem.h>
#include <openssl/hmac.h>

using namespace std;

 
size_t base64Encode(const void *data, int data_len, char *buffer)
{
	BIO *b64 = BIO_new(BIO_f_base64());
	BIO *bio = BIO_new(BIO_s_mem());
 
	bio = BIO_push(b64, bio);
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, data, data_len);
	BIO_ctrl(bio, BIO_CTRL_FLUSH, 0, NULL);
 
	BUF_MEM *bptr = NULL;
	BIO_get_mem_ptr(bio, &bptr);
 
	size_t slen = bptr->length;
	memcpy(buffer, bptr->data, slen);
	buffer[slen] = '\0';
 
	BIO_free_all(bio);
	return slen;
}
 
int HmacEncode(const char * algo,
	const char * key, unsigned int key_length,
	const char * input, unsigned int input_length,
	string &strOutput, unsigned int &output_length) {
	const EVP_MD * engine = NULL;
	if (strcmp("sha512", algo) == 0) {
		engine = EVP_sha512();
	}
	else if (strcmp("sha256", algo) == 0) {
		engine = EVP_sha256();
	}
	else if (strcmp("sha1", algo) == 0) {
		engine = EVP_sha1();
	}
	else if (strcmp("md5", algo) == 0) {
		engine = EVP_md5();
	}
	else if (strcmp("sha224", algo) == 0) {
		engine = EVP_sha224();
	}
	else if (strcmp("sha384", algo) == 0) {
		engine = EVP_sha384();
	}
	else {
		cout << "Algorithm " << algo << " is not supported by this program!" << endl;
		return -1;
	}
 
	unsigned char* output = (unsigned char*)malloc(EVP_MAX_MD_SIZE);
 
	HMAC_CTX *ctx = HMAC_CTX_new();
	HMAC_Init_ex(ctx, key, strlen(key), engine, NULL);
	HMAC_Update(ctx, (unsigned char*)input, strlen(input));        // input is OK; &input is WRONG !!!
 
	HMAC_Final(ctx, output, &output_length);
	HMAC_CTX_free(ctx);
 
	strOutput = (char*)output;
 
	return 0;
}

// 将请求返回的数据写在本地文件
size_t HttpsReturnMsg(void* buffer, size_t size, size_t nmemb, void *stream)
{
    cout << "\n\n-------- Original HttpsReturnMsg --------" << endl;
    cout << (char*)buffer << endl;

	// FILE *fptr = (FILE*)stream;
	// fwrite(buffer, size, nmemb, fptr);
	return size*nmemb;
}

int test_post_https_official()
{
	CURL *curl;
	CURLcode res;
 
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
 
	FILE* fp = fopen("e:/1.txt", "wb+");
 
	if (curl)
	{
		// 参考连接：https://yq.aliyun.com/articles/629830?spm=a2c4e.11153940.blogcont630369.18.3dca5d3c1QHa9l
		string Url = "https://api.uat.b2c2.net";
		// sprintf(Url, "https://yq.aliyun.com/articles/629830?spm=a2c4e.11153940.blogcont630369.18.3dca5d3c1QHa9l");

		curl_slist *pList = NULL;
		pList = curl_slist_append(pList, "Accept:application/json, text/plain, */*");
		pList = curl_slist_append(pList, "Content-Type:application/json;charset=UTF-8");
		pList = curl_slist_append(pList, "X-Ca-Key:12345678");
 
		string appKey = "12345678";
		string appSecret = "TEST1234ABCD5678AAAA";
		string customHeaders = "x-ca-key:" + appKey + "\n";
		string httpHeaders = "POST\napplication/json, text/plain, */*\napplication/json;charset=UTF-8\n";
		string strNeedEncode = httpHeaders + customHeaders + "articles/629830?spm=a2c4e.11153940.blogcont630369.18.3dca5d3c1QHa9l";
 
		string strMac;
		unsigned int mac_length = 0;
 
		int ret = HmacEncode("sha256", appSecret.c_str(), appSecret.length(), strNeedEncode.c_str(), strNeedEncode.length(), strMac, mac_length);
		char *p = new char[128];
		memset(p, 0, 128);
		base64Encode(strMac.c_str(), mac_length, p);
		
		char strX_Ca_Signature[256] = { 0 };
		sprintf(strX_Ca_Signature, "X-Ca-Signature:%s", p);
 
		pList = curl_slist_append(pList, strX_Ca_Signature);
		pList = curl_slist_append(pList, "X-Ca-Signature-Headers:x-ca-key");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pList);
 
		curl_easy_setopt(curl, CURLOPT_URL, Url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1); // post req  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpsReturnMsg);	//对返回的数据进行操作的函数地址
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);					//这是HttpsReturnMsg的第四个参数值
 
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
		char strMsgBody[1024] = { 0 };
		// sprintf(strMsgBody, 1024, "{\"name\":\"hhhh\",\"age\":\"12\"}");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strMsgBody);
 
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
 
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
 
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
 
	return 0;

     
}

int test_post_https()
{
	CURL *curl;
	CURLcode res;
 
	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
 
	FILE* fp = fopen("e:/1.txt", "wb+");
 
	if (curl)
	{
		// 参考连接：https://yq.aliyun.com/articles/629830?spm=a2c4e.11153940.blogcont630369.18.3dca5d3c1QHa9l
		string Url = "https://api.uat.b2c2.net/order/";
		// sprintf(Url, "https://yq.aliyun.com/articles/629830?spm=a2c4e.11153940.blogcont630369.18.3dca5d3c1QHa9l");

		curl_slist *pList = NULL;
		// pList = curl_slist_append(pList, "Accept:application/json, text/plain, */*");
		// pList = curl_slist_append(pList, "Content-Type:application/json;charset=UTF-8");
		pList = curl_slist_append(pList, "Authorization:Token eabe0596c453786c0ecee81978140fad58daf881");

		pList = curl_slist_append(pList, "Accept:*/*");
		pList = curl_slist_append(pList, "Content-Type:application/json");		
 
		// string appKey = "12345678";
		// string appSecret = "TEST1234ABCD5678AAAA";
		// string customHeaders = "x-ca-key:" + appKey + "\n";
		// string httpHeaders = "POST\napplication/json, text/plain, */*\napplication/json;charset=UTF-8\n";
		// string strNeedEncode = httpHeaders + customHeaders + "articles/629830?spm=a2c4e.11153940.blogcont630369.18.3dca5d3c1QHa9l";
 
		// string strMac;
		// unsigned int mac_length = 0;
 
		// int ret = HmacEncode("sha256", appSecret.c_str(), appSecret.length(), strNeedEncode.c_str(), strNeedEncode.length(), strMac, mac_length);
		// char *p = new char[128];
		// memset(p, 0, 128);
		// base64Encode(strMac.c_str(), mac_length, p);
		
		// char strX_Ca_Signature[256] = { 0 };
		// sprintf(strX_Ca_Signature, "X-Ca-Signature:%s", p);
 
		// pList = curl_slist_append(pList, strX_Ca_Signature);
		// pList = curl_slist_append(pList, "X-Ca-Signature-Headers:x-ca-key");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, pList);
 
		curl_easy_setopt(curl, CURLOPT_URL, Url.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1); // post req  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpsReturnMsg);	//对返回的数据进行操作的函数地址
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);					//这是HttpsReturnMsg的第四个参数值
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); 
 
		/* example.com is redirected, so we tell libcurl to follow redirection */
		// curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
 
		// char strMsgBody[1024] = { 0 };
		// sprintf(strMsgBody, 1024, "{\"name\":\"hhhh\",\"age\":\"12\"}");

        string strJsonData;
        strJsonData = "client_order_id=1010&";
        strJsonData += "quantity=10&" ;
        strJsonData += "side=buy&" ;
        strJsonData += "instrument=XRP.SOPT&" ;
        strJsonData += "order_type=FOK&" ;
        strJsonData += "price=0.64&" ;
        strJsonData += "valid_until=2021-07-19T03:41:01&" ;
        strJsonData += "executing_unit=risk-adding-strategy" ;

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strJsonData.c_str());  
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strJsonData.size());


		// curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strMsgBody);
 
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);

		cout << "Returen Response " << res << endl;
 
		/* Check for errors */
		if (res != CURLE_OK)
        {
            cout << "\n+++++++++++  curl response " << res << endl;
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
			
 
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();
 
	return 0;     
}


void TestCurl()
{
    cout << "\n\n----------- TEST CURL -------------" << endl;
    // test_post_create_order();

    test_post_https();
}