#pragma once
#include "IMonitor.h"
#include "core/ThreadSafeQueue.h"
#include "data/Models.h"
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>

class NetworkMonitor : public IMonitor
{
public:
    bool Start() override;
    void Stop() override;
    std::vector<NetworkConnection> GetConnections();

private:
    void Loop();
    void Refresh();

    std::atomic<bool> running_{ false };
    std::thread thread_;
    std::mutex mutex_;
    std::vector<NetworkConnection> connections_;
};