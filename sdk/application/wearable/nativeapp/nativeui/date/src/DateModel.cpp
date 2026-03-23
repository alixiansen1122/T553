/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: DateModel
 * Create: 2025-03-23
 */

#include "bts_br_gap.h"
#include "bts_spp.h"
#include "msg_center_customer.h"
#include "msg_center_device.h"
#include "ohos_timer.h"
#include "phone_msg_notification.h"
#include "date/DatePresenter.h"
#include "date/DateModel.h"

namespace OHOS {
static constexpr int16 INDEX_0 = 0;
static constexpr int16 INDEX_1 = 1;
static constexpr int16 INDEX_2 = 2;
static constexpr int16 INDEX_3 = 3;
static constexpr int16 INDEX_4 = 4;
static constexpr int16 INDEX_5 = 5;
static const char *DATE_FILE = DATE_PATH "/calendar.json";

DateModel::DateModel() {}

DateModel::~DateModel() {}

DateModel &DateModel::GetInstance()
{
    static DateModel instance;
    return instance;
}

bool DateModel::InitDateVec(std::string dateStr)
{
    int index = 0;
    listDate_.clear();
    if (GetConnectStatus() == 0) {
        listDate_.push_back({"无日程", "请连接手机，同步日程信息", "0", "0", true});
    } else {
        for (const auto &msg : keyValues_) {
            if (msg.first != dateStr) {
                continue;
            }
            for (const auto &item : msg.second) {
                listDate_.push_back(
                    {item.hour.c_str(), item.eventTitle.c_str(), item.address.c_str(), item.index.c_str(), false});
            }
        }
        
        if (listDate_.size() == 0) {
            listDate_.push_back({"无日程", "请前往手机日历创建日程", "0", "0", true});
        }
    }
    return true;
}

uint8_t DateModel::GetConnectStatus()
{
    uint8_t conn_status = 0;
#ifdef _WIN32
    conn_status = 1;
#else
    if (msg_center_get_trans_channel() == DIAG_FRAME_FID_PHONE) {
        conn_status = is_spp_connected(0) == true ? 1 : 0;
    } else if (msg_center_get_trans_channel() == DIAG_FRAME_FID_BT) {
        conn_status = msg_center_get_ble_conn_state();
    }
#endif
    return conn_status;
}

bool DateModel::InitDate()
{
    keyValues_.clear();
    dateClockVec_.clear();
    std::ifstream file(DATE_FILE);
    if (!file.is_open()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "cannot open file");
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    int intsize = sizeof(content);
    cJSON *root = nullptr;
    if (intsize != 0) {
        root = cJSON_Parse(content.c_str());
    }
    if (root == NULL) {
        const char *error_ptr;
        return false;
    }
    cJSON *dateSum = cJSON_GetObjectItemCaseSensitive(root, "dateSum");
    if (!cJSON_IsObject(dateSum)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error observe:");
        cJSON_Delete(root);
        return false;
    }
    int sum = std::atoi(cJSON_GetObjectItemCaseSensitive(dateSum, "count")->valuestring);
    for (int i = 0; i < sum; i++) {
        cJSON *date = cJSON_GetObjectItemCaseSensitive(dateSum, std::to_string(i).c_str());
        if (!cJSON_IsObject(date)) {
            cJSON_Delete(root);
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error date");
            return false;
        }

        std::string time = "测试time";
        if (!cJSON_GetObjectItemCaseSensitive(date, "time")) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error time:");
        } else {
            time = cJSON_GetObjectItemCaseSensitive(date, "time")->valuestring;
        }

        std::string hour = "测试hour";
        if (!cJSON_GetObjectItemCaseSensitive(date, "hour")) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error hour:");
        } else {
            hour = cJSON_GetObjectItemCaseSensitive(date, "hour")->valuestring;
        }

        std::string eventTitle = "测试eventTitle";
        if (!cJSON_GetObjectItemCaseSensitive(date, "eventTitle")) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error eventTitle:");
        } else {
            eventTitle = cJSON_GetObjectItemCaseSensitive(date, "eventTitle")->valuestring;
        }

        std::string address = "no location";
        if (!cJSON_GetObjectItemCaseSensitive(date, "location")) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Error location:");
        } else {
            address = cJSON_GetObjectItemCaseSensitive(date, "location")->valuestring;
        }
        std::string index = std::to_string(i);
        DateMsg item = {hour, eventTitle, address, index};
        keyValues_[time].push_back(item);
        std::string start_time = hour.substr(0, INDEX_5);
        std::string result_time = time + " " + start_time;
        eventTitle = hour + "\r\n" + eventTitle;
        CalcClockTime(result_time, eventTitle);
    }
    cJSON_Delete(root);
    return true;
}

void DateModel::CalcClockTime(std::string result_time, std::string eventTitle)
{
    std::tm tm = {};
    std::istringstream ss("2025-06-18 15:20");
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    if (ss.fail()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CalcClockTime cannot open file");
        return;
    }
    DateToMessage item;
    item.time = result_time;
    item.eventTitle = eventTitle;
    dateClockVec_.push_back(item);
}

void DateModel::Init()
{
    InitDateTimer();
}

bool HasFileChanged(const std::string &filePath, time_t &lastModTime, std::string &lastContent)
{
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) != 0) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "cat file fail");
        return false;
    }

    if (fileStat.st_mtime == lastModTime) {
        return false;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "cannot open file");
        return false;
    }

    std::string currentContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (currentContent == lastContent) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "updateLastModTime");
        lastModTime = fileStat.st_mtime;
        return false;
    }

    lastModTime = fileStat.st_mtime;
    lastContent = currentContent;
    return true;
}

std::vector<int> parseTime(const std::string &s)
{
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    char sep;
    std::istringstream ss(s);
    ss >> year >> sep >> month >> sep >> day >> hour >> sep >> minute;
    if (s.size() > TIME_END) {
        ss >> sep >> second;
    }
    return {year, month, day, hour, minute, second};
}

void CmpClock()
{
    std::time_t now = std::time(nullptr);
    std::tm *local = std::localtime(&now);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
    std::string time_str(buffer);
    for (auto item = DateModel::GetInstance().dateClockVec_.begin();
         item != DateModel::GetInstance().dateClockVec_.end();) {
        auto vsec = parseTime(item->time);
        auto localtime = parseTime(time_str);
        if (vsec[INDEX_0] < localtime[INDEX_0]) {
            item = DateModel::GetInstance().dateClockVec_.erase(item);
            continue;
        } else if (vsec[INDEX_0] > localtime[INDEX_0]) {
            ++item;
            continue;
        }

        if (vsec[INDEX_1] < localtime[INDEX_1]) {
            item = DateModel::GetInstance().dateClockVec_.erase(item);
            continue;
        } else if (vsec[INDEX_1] > localtime[INDEX_1]) {
            ++item;
            continue;
        }

        if (vsec[INDEX_2] < localtime[INDEX_2]) {
            item = DateModel::GetInstance().dateClockVec_.erase(item);
            continue;
        } else if (vsec[INDEX_2] > localtime[INDEX_2]) {
            ++item;
            continue;
        }
        auto diff = (vsec[INDEX_3] - localtime[INDEX_3]) * NUM_TIME_HOUR +
                    (vsec[INDEX_4] - localtime[INDEX_4]) * NUM_TIME_MIN + (vsec[INDEX_5] - localtime[INDEX_5]);
        if (diff <= NUM_TIME_MIN && diff > 0) {
#ifdef _WIN32
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PostGraphicEvent date msg");
#else
            GraphicService::GetInstance()->PostGraphicEvent(
                std::bind(&PhoneMsgView::ShowDateMsg, "日历", "日历通知", item->eventTitle));
            item = DateModel::GetInstance().dateClockVec_.erase(item);
#endif
        } else if (diff < 0) {
            item = DateModel::GetInstance().dateClockVec_.erase(item);
        } else {
            ++item;
        }
    }
}

void DateModel::ClearData()
{
    std::remove(DATE_FILE);
}

/* 定时器回调函数 */
void DateTimerCallback(void *data)
{
    (void)data;
    static time_t lastModTime = 0;
    static std::string lastContent;
    bool change = HasFileChanged(DATE_FILE, lastModTime, lastContent);
    if (change) {
        DateModel::GetInstance().InitDate();
    }
    if (DateModel::GetInstance().dateClockVec_.size() > 0) {
        CmpClock();
    }
}

/* 开启定时器 */
void DateModel::InitDateTimer()
{
    if (startCount_ != nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DatePhoneMenuPhoneMainPage already exists!!");
        return;
    }
    uint32 tick = GetOSTick(NUM_THIRTY * MS_ONE_SECONDS);
    startCount_ = new GraphicTimer(tick, DateTimerCallback, nullptr, true);
    if (startCount_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DatePhoneMenuPhoneMainPage new GraphicTimer failed!!");
        return;
    }
    bool retTimer = startCount_->Start();
    if (!retTimer) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "DatePhoneMenuPhoneMainPage osTimerStart failed!!");
        if (startCount_ != nullptr) {
            delete startCount_;
            startCount_ = nullptr;
        }
        return;
    }
}

/* 关闭定时器 */
void DateModel::DisableDatePhoneTimer()
{
    if (startCount_ != nullptr) {
        startCount_->Stop();
        delete startCount_;
        startCount_ = nullptr;
    }
}
}
