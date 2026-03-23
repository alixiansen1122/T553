#ifndef DOWNLOAD_MANAGER_H
#define DOWNLOAD_MANAGER_H

#include "cmsis_os2.h"
#include "http_module_client.h"
#include "vector"
#include "string"

namespace OHOS {
namespace ACELite {

// 下载任务状态
enum DownloadStatus {
    STATUS_PENDING = 0,   // 等待中
    STATUS_RUNNING = 1,   // 执行中
    STATUS_SUCCESS = 2,   // 成功
    STATUS_FAILED = 3,    // 失败
    STATUS_CANCELED = 4,   // 已取消
    STATUS_REPEAT = 5
};

enum DownloadManagerId {
    DOWNLOAD_MANAGER_INIT,
    DOWNLOAD_MANAGER_EXIT,
    DOWNLOAD_MANAGER_STOP,
    DOWNLOAD_MANAGER_START,
};

struct DownloadManagerMessage {
    DownloadManagerId requestId;
    void * userData;
};

// 下载任务结构体
struct DownloadTask {
    std::string url;                  // 下载URL
    std::string filePath;             // 保存路径
    std::string header;               // HTTP请求头
    HttpRequestId requestId;          // HTTP请求类型
    DownloadStatus status;            // 任务状态
    HttpErrcode errCode;              // 错误码（失败时有效）
    void *caller;                     // 任务所属 
    std::function<void(DownloadTask&)> callback;  // 任务结果回调
    DownloadTask(const std::string& u, const std::string& path, 
                 const std::string& h, const std::function<void(DownloadTask&)>& cb, HttpRequestId retId, void *id)
        : url(u), filePath(path), header(h), status(STATUS_PENDING), requestId(retId), caller(id),
          errCode(NET_UNKNOWN_ERROR), callback(cb) {}
};

class DownloadManager {
public:
    // 单例模式（全局唯一管理器）
    static DownloadManager& GetInstance() {
        static DownloadManager instance;
        return instance;
    }

    bool IsInited(void) const
    {
        return isInit_ ;
    }

    void SetInit(bool isInit) 
    {
        isInit_ = isInit;
    }

    // 禁止拷贝构造和赋值
    DownloadManager(const DownloadManager&) = delete;
    DownloadManager& operator=(const DownloadManager&) = delete;

    // 启动管理器（初始化线程和客户端）
    bool Start();
    bool Init();
    // 停止管理器（清空队列，终止线程）
    void Stop();
    void Exit();
    
    // 添加下载任务到队列（线程安全）
    void AddTask(const DownloadTask& task);

    // 取消指定task的任务（线程安全）
    bool CancelTask(DownloadTask& task);

    void RestTask();
    // 获取当前任务队列状态
    std::vector<DownloadTask> GetTaskQueue() const;

private:
    DownloadManager();
    ~DownloadManager();
    bool InithttpClient();
    // 管理器线程入口
    static void ManagerThread(void* argument);
    osStatus_t DownloadManagerMessagePut(DownloadManagerMessage *message);
    osStatus_t DownloadManagerMessageGet(DownloadManagerMessage *message);
    // 处理任务队列（核心逻辑）
    void ProcessTasks();
    void StopHttp();
    mutable osMutexId_t queueMutex_;   // 保护任务队列的互斥锁
    
    osSemaphoreId_t queueSem_;        // 任务队列条件变量（用于线程Pend）
    osThreadId_t managerThreadId_;     // 管理器线程ID
    bool isRunning_;                   // 管理器运行状态
    std::vector<DownloadTask> taskQueue_;  // 任务队列（串行执行）
    HttpModuleClient httpClient_;      // HTTP下载客户端
    osSemaphoreId_t taskSem_;         // 任务完成条件变量
    bool currentTaskFinished_;         // 当前任务是否完成
    bool currentTaskSucceeded_;        // 当前任务是否成功
    bool isInit_;        // 当前任务是否成功
    osMessageQueueId_t ManagerQueueId_ = nullptr;
    void * caller_ = nullptr;
};

}  // namespace ACELite
}  // namespace OHOS

#endif  // DOWNLOAD_MANAGER_H