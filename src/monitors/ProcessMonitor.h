#pragma once
#include "IMonitor.h"
#include "data/Models.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <windows.h>

class ProcessMonitor : public IMonitor
{
public:
    bool Start() override;
    void Stop() override;
    std::vector<ProcessInfo> GetProcesses();
    bool KillProcess(DWORD pid);
    bool SuspendProcess(DWORD pid);
    bool ResumeProcess(DWORD pid);

private:
    void Loop();
    void Refresh();

    std::atomic<bool> running_{ false };
    std::thread thread_;
    std::mutex mutex_;
    std::unordered_map<DWORD, ProcessInfo> processes_;
    ULONGLONG lastRefresh_ = 0;
};