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
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);

    cout << "ori_data: " << (char*)(ptr) << "\n"
        << "size: " << size << "\n"
        << "nmemb: " << nmemb << endl;

    if (mem->memory) 
    {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
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
    strJsonData += "executing_unit=risk-adding-strategy&" ;

    //libcur的相关POST配置项
    curl_easy_setopt(pCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strJsonData.c_str());  
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDSIZE, strJsonData.size());


    res = curl_easy_perform(pCurl);

    long res_code=0;
    res=curl_easy_getinfo(pCurl, CURLINFO_RESPONSE_CODE, &res_code);

    if(( res == CURLE_OK ) && (res_code == 200 || res_code == 201))
    {
        FILE* fTmpMem = (FILE*)oDataChunk.memory;
        if (!fTmpMem) {

        }
        FILE *fp=fopen(filename.c_str(),"wb");
        if(!fp)
        {   
            cout<<"open file failed";
            return -1;
        }   

        fwrite(fTmpMem, 1, oDataChunk.size, fp);
        fclose(fp);
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


void TestCurl()
{
    cout << "\n\n----------- TEST CURL -------------" << endl;
    test_post_create_order();
}