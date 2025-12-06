#include "ute_server.h"

#include "write_message_manager.h"
#include "share_comm_external_message.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utility>

#include <vector>
#include <algorithm>

#include "json_struct.h"

using namespace share_common;

#define UTE_MANAGER ThreadSafeSingleton<WriteMessageManager>::DoubleCheckInstance()

int gTestCount = 10000;
int gTestIndex = 0;
int gTestMode = 0; // 0-正常测试；1-压力性能测试;
std::vector<unsigned long long> gTestTimeVec;

JsonMeta          gJsonMeta;



struct KLineData {
    std::string strSymbol_;
    long long llOpenTime_;
    double dOpen_;
    double dHigh_;
    double dLow_;
    double dClose_;
    double dVolume_;
    double dQuoteVolume_;
    long long llCloseTime_;
    double dNumTrades_;
    double d60Ave_;
    double stdev_;
};



// 特殊字符转义函数（复用之前的实现，确保格式正确）
std::string escape_csv_field(const std::string& field) {
    bool need_quote = (field.find(',') != std::string::npos) ||
                      (field.find('"') != std::string::npos) ||
                      (field.find('\n') != std::string::npos) ||
                      (field.find('\r') != std::string::npos);
    if (!need_quote) return field;

    std::string escaped = field;
    std::replace(escaped.begin(), escaped.end(), '"', '"');
    return "\"" + escaped + "\"";
}

// 增量写入单条数据（核心函数）
bool append_to_csv(const std::string& filename, const KLineData& kline) {
    // 以追加模式打开文件（ios::app + ios::out），二进制模式避免换行符转换
    std::ofstream csv_file(filename, std::ios::out | std::ios::app | std::ios::binary);
    if (!csv_file.is_open()) {
        std::cerr << "Error: 无法打开文件 " << filename << std::endl;
        return false;
    }

    // 若文件为空且需要表头，先写入表头（首次运行时）
    csv_file.seekp(0, std::ios::end);  // 移动到文件末尾

    // 写入数据（控制浮点数精度，避免科学计数法）
    csv_file << std::fixed << std::setprecision(2);
    csv_file << escape_csv_field(kline.strSymbol_) << ","
             << escape_csv_field(ToSecondStr(kline.llOpenTime_)) << ","
             << kline.dOpen_ << ","
             << kline.dHigh_ << ","
             << kline.dLow_ << ","
             << kline.dClose_ << ","
             << kline.dVolume_ << ","
             << kline.dQuoteVolume_ << ","
             << kline.dNumTrades_ << ","
             << kline.d60Ave_ << ","
             << kline.stdev_ << "\n";

    csv_file.close();
    return true;
}

void init_csv_file(const std::string& filename) {
    std::ofstream csv_file(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!csv_file.is_open()) {
        std::cerr << "Error: 无法打开文件 " << filename << std::endl;
        return;
    }
    csv_file << "证券代码,时间戳,开盘价,最高价,最低价,收盘价,成交量,成交额,60日均线,标准差\n";
    csv_file.close();
}

class ClacTickData {
public:


    ClacTickData() {

        init_csv_file(strCsvFileName);

        vecTickData_.clear();
        vecKLineData_.clear();
    };
    ~ClacTickData() {

    };



    unsigned long long curMiniuteTime(unsigned long long tickTime) {
        return tickTime / 600000000000 * 600000000000;
    }

    // 计算 60日均线数据，标准差数据;
    void compute_60_ave_stdev(std::vector<KLineData>& vecKlineData) {
        if (vecKlineData.size() < 60) {
            vecKlineData[vecKlineData.size() - 1].d60Ave_ = vecKlineData[vecKlineData.size() - 1].stdev_ = 0;
            return;
        }

        double sum = 0;
        for (int i = 0; i < 60; ++i) {
            sum += vecKlineData[i].dClose_;
        }
        vecKlineData[vecKlineData.size() - 1].d60Ave_ = sum / 60;
        double sum_sq = 0;
        for (int i = 0; i < 60; ++i) {
            sum_sq += (vecKlineData[i].dClose_ - vecKlineData[vecKlineData.size() - 1].d60Ave_) * (vecKlineData[i].dClose_ - vecKlineData[vecKlineData.size() - 1].d60Ave_);
        }
        vecKlineData[vecKlineData.size() - 1].stdev_ = std::sqrt(sum_sq / 60);
    }

    void ComputeKLineData() {
        KLineData kLineData;

        if (vecTickData_.size() > 0) {
            for (auto& tickData : vecTickData_) {
                kLineData.dHigh_ = std::max(kLineData.dHigh_, tickData.dPrice);
                kLineData.dLow_ = std::min(kLineData.dLow_, tickData.dPrice);
                kLineData.dClose_ = tickData.dPrice;
                kLineData.dVolume_ += tickData.dVolume;
                kLineData.dQuoteVolume_ += tickData.dTurnover;
                kLineData.dNumTrades_ += tickData.dVolume;
            }
            kLineData.llCloseTime_ = vecTickData_[vecTickData_.size() - 1].dTickTime;
            kLineData.llOpenTime_ = vecTickData_[0].dTickTime;  
            kLineData.strSymbol_ = vecTickData_[0].strSymbol;
            vecKLineData_.push_back(kLineData);
        }



        append_to_csv(strCsvFileName, kLineData);

        vecTickData_.clear();
    }

    void UpdateTickData(const TickData& tickData) {
        

        if (vecTickData_.size() > 0) {
            if (curMiniuteTime(tickData.dTickTime) != curMiniuteTime(vecTickData_[vecTickData_.size() -1].dTickTime)) {
                ComputeKLineData();
            }
        }

        vecTickData_.push_back(tickData);
    }

    std::vector<TickData> vecTickData_;

    std::vector<KLineData> vecKLineData_;

    std::string strCsvFileName = "kline_data.csv";
};

ClacTickData gClacTickData;

void WriteOnEvent(int iMsgID, const char* pMsgBuf,  unsigned long long ulReadKey) {
    LOG_INFO("WriteOnEvent: iMsgID={}, ulReadKey={}", iMsgID, ulReadKey);

    
}


void WriteOnMessage(int iMsgID, const char* pMsgBuf, unsigned long long ulReadKey)  {
    TickData* pTickData = (TickData*)pMsgBuf;
    LOG_INFO("WriteOnMessage: iMsgID={}, ulReadKey={}, {}", iMsgID, ulReadKey, pTickData->str());

    gClacTickData.UpdateTickData(*pTickData);
}

void TcpFunc() {
    // LOG_INFO("TcpFunc");
}

void TestWriteServer() {
    gTestMode = 0;

    gTestTimeVec.reserve(gTestCount);
    for (int i = 0; i<gTestCount; ++i) {
        gTestTimeVec.push_back(0);
    }

    int iEventSleepSec = 5;

    UTE_MANAGER->SetOnEvent(iEventSleepSec, WriteOnEvent);
    UTE_MANAGER->SetOnMessage(WriteOnMessage);

    UTE_MANAGER->Init(gJsonMeta.strWriteName_.c_str());


    // sleep(5);
    // SendApiMessage();
    // sleep(1);
    // SendApiMessage();    
}

void TestWriteMain() {
    // const string strSrcJsonFileName = "test_data.json";
    // JSON_HELPER->Init(strSrcJsonFileName);

    std::string strMetaInfo = gJsonMeta.Init("meta_data.json");
    LOG_INFO("MetaInfo:\n{}", strMetaInfo);    

    TestWriteServer();

    while(true) {
        sleep(10);
    }
}