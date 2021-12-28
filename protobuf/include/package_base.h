#pragma once

#include "global_declare.h"
#include <sys/time.h>
#include "channel.h"
#include "time_util.h"

// message chan flag
const char CHAIN_CONTINUE	    = 'C';		//　not last package
const char CHAIN_LAST		    = 'L';		// last package

const unsigned char UT_REQUEST	 = 'R';		// 请求
const unsigned char UT_RESPONSE	 = 'O';		// 应答
const unsigned char UT_SUBSCRIBE = 'S';		// 订阅
const unsigned char UT_PUBLISH	 = 'P';		// 发布
const unsigned char UT_UNKNOWN	 = 'U';		// 未知

struct PackageHead
{
    PackageHead() {}
    // package session identity
    std::string SessionID{""};
    std::string RpcID{""};
    // package type (request, response or publish)
    unsigned char Type{UT_UNKNOWN};
    // message id
    unsigned int Tid{0};
    // chain last?
    unsigned char	Chain{CHAIN_LAST};
    // RequestID
    int64_t RequestID{0};
    // sequence number
    long SequenceNo{0};
    // Package ID（全局的数据包编号）
    long PackageID{0};
    // channel name
    std::string Channel{""};
    // package create time
    long CreateTime{0};
    // physical name
    std::string Physical{""};
    // access token (TODO: to remove when we don't use redis to send data)
    std::string AccessToken{""};
};

// specially for handle with package head
struct PackageBase
{
    PackageBase()
    {
        Head.CreateTime = NanoTime();
        // std::cout << "init package time " << Head.CreateTime << std::endl;
    }
    virtual ~PackageBase(){}

    void prepare_request(const unsigned int tid, int64_t request_id, unsigned char chain=CHAIN_LAST, const std::string& sessionid="")
    {
        Head.SessionID = sessionid;
        Head.Type = UT_REQUEST;
        Head.Tid = tid;
        Head.Chain = chain;
        Head.RequestID = request_id;
    }

    void prepare_response(const unsigned int tid, int64_t request_id, unsigned char chain=CHAIN_LAST, const std::string& sessionid="")
    {
        Head.SessionID = sessionid;
        Head.Type = UT_RESPONSE;
        Head.Tid = tid;
        Head.Chain = chain;
        Head.RequestID = request_id;
    }

    void prepare_response(PackageBase* package, const unsigned int tid)
    {
        Head.SessionID = package->SessionID();
        Head.Type = UT_RESPONSE;
        Head.Tid = tid;
        Head.Chain = CHAIN_LAST;
        Head.RequestID = package->RequestID();
        Head.Channel = package->Channel();
    }

    void prepare_subscribe(const unsigned int tid, const std::string& sessionid="")
    {
        Head.SessionID = sessionid;
        Head.Type = UT_SUBSCRIBE;
        Head.Tid = tid;
        Head.Chain = CHAIN_LAST;
    }

    void prepare_publish(const unsigned int tid, unsigned char chain=CHAIN_LAST, const std::string& sessionid="")
    {
        Head.SessionID = sessionid;
        Head.Type = UT_PUBLISH;
        Head.Tid = tid;
        Head.Chain = chain;
        Head.RequestID = 0;
        Head.SequenceNo = 0;
        Head.Channel = "";
    }

    void send_head(boost::shared_ptr<IChannel> sender, bool pack_session_id, bool is_last=false)
    {
        if (pack_session_id)
            sender->send(Head.SessionID, is_last);
        sender->send(Head.Tid, is_last);
        sender->send(Head.Type, is_last);
        sender->send(Head.Chain==CHAIN_LAST?1:0, is_last);
        sender->send(Head.RequestID, is_last);
        sender->send(Head.SequenceNo, is_last);
        sender->send(Head.CreateTime, is_last);
        sender->send(Head.Channel, is_last);
        // std::cout << "[SEND_HEAD] [SessionID=" << Head.SessionID << "] [Tid=" << Head.Tid << "] [RequestID=" << Head.RequestID << "] [Chain=" << Head.Chain << "] ";
    }

    void read_head(boost::shared_ptr<IChannel> reader, bool pack_session_id)
    {
        if (pack_session_id)
            reader->read(Head.SessionID);
        reader->read(Head.Tid);
        reader->read(Head.Type);
        std::string chain_sign;
        reader->read(chain_sign);
        Head.Chain = chain_sign=="1" ? CHAIN_LAST : CHAIN_CONTINUE;
        reader->read(Head.RequestID);
        reader->read(Head.SequenceNo);
        reader->read(Head.CreateTime);
        reader->read(Head.Channel);
        // std::cout << "[READ_HEAD] [SessionID=" << Head.SessionID << "] [Tid=" << Head.Tid << "] [RequestID=" << Head.RequestID << "] [Chain=" << Head.Chain << "] ";
    }

    // this package is request package ?
    bool is_request_package()
    {
        return Head.Type == UT_REQUEST;
    }

    // retrieve the current time
    string time_now()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        auto tm_time = gmtime(&tv.tv_sec);
        char now[32] = {0};
        sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d.%6ld", tm_time->tm_year + 1900, tm_time->tm_mon+1, tm_time->tm_mday, tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec, tv.tv_usec);
        return now;
    }

    // head str
    std::string head_str()
    {
        std::string head;
        head += "[SessionID=" + Head.SessionID + "] ";
        head += "[Type=" + std::to_string(Head.Type) + "] ";
        char tid[12]={'\0'}; sprintf(tid, "0x%08x", Head.Tid);
        head += "[Tid=" + std::string(tid) + "] ";
        head += "[Chain=" + std::to_string(Head.Chain) + "] ";
        head += "[RequestID=" + std::to_string(Head.RequestID) + "] ";
        head += "[SequenceNo=" + std::to_string(Head.SequenceNo) + "] ";
        head += "[Channel=" + Head.Channel + "] ";
        head += "[CreateTime=" + std::to_string(Head.CreateTime) + "] ";
        head += "[PhysicalName=" + Head.Physical + "] ";
        return head;
    }

    // retrieve the message type
    unsigned int Tid(){ return Head.Tid; }
    // retrieve the strategy name
    std::string  SessionID(){ return Head.SessionID; }

    std::string  RpcID(){ return Head.RpcID; }

    // retrieve the request identity number
    int64_t         RequestID(){ return Head.RequestID; }
    // is the last response
    bool         IsLast(){ return Head.Chain==CHAIN_LAST; }
    // chain
    unsigned char Chain() { return Head.Chain; }
    // type
    unsigned char Type(){ return Head.Type; }
    // SequenceNo
    long SequenceNo(){ return Head.SequenceNo; }
    // channel
    std::string Channel() { return Head.Channel; }
    // physical account name
    std::string Physical() { return Head.Physical; }
    // create time
    long CreateTime() { return Head.CreateTime; }
    // access token information
    std::string AccessToken() { return Head.AccessToken; }
    // package ID
    long PackageID() { return Head.PackageID; }

    // set request id
    void SetRequestID(int64_t request_id) { Head.RequestID=request_id; }
    // set chain
    void SetChain(unsigned char Chain) { Head.Chain = Chain; }
    // set channel field
    void SetChannel(const std::string& channel) { Head.Channel = channel; }
    // set sequence number
    void SetSequenceNo(long sequenceNo) { Head.SequenceNo=sequenceNo; }
    // set physical account name
    void SetPhysical(const string& physical) { Head.Physical=physical; }
    // reset Create Time
    void SetCreateTime(const long& create_time) { Head.CreateTime = create_time; }
    // set Access Token
    void SetAccessToken(const std::string& access_token) { Head.AccessToken=access_token; }
    // set the session identity
    void SetSessionID(const std::string& session_id) { Head.SessionID=session_id; }

    void SetRpcID(const std::string& rpc_id) { Head.RpcID = rpc_id; }
    // global package id
    void SetPackageID(const long& package_id) { Head.PackageID=package_id; }
    // reset the package
    virtual void reset_fields() = 0;
    // field head struct
    PackageHead Head;
};