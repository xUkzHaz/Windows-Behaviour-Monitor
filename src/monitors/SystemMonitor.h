#pragma once
#include "IMonitor.h"
#include "data/Models.h"
#include <atomic>
#include <thread>
#include <mutex>

class SystemMonitor : public IMonitor
{
public:
    bool Start() override;
    void Stop() override;
    SystemStats GetStats();

private:
    void Loop();

    std::atomic<bool> running_{ false };
    std::thread thread_;
    std::mutex mutex_;
    SystemStats stats_;

    ULONGLONG prevIdle_ = 0;
    ULONGLONG prevKernel_ = 0;
    ULONGLONG prevUser_ = 0;
};