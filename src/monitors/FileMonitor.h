#pragma once
#include "IMonitor.h"
#include "core/ThreadSafeQueue.h"
#include "data/Models.h"
#include <atomic>
#include <thread>
#include <vector>
#include <windows.h>

class FileMonitor : public IMonitor
{
public:
    bool Start() override;
    void Stop() override;
    ThreadSafeQueue<FileEvent>& Events() { return events_; }
    void WatchDirectory(const std::wstring& path);

private:
    void WatchLoop(std::wstring path);
    std::atomic<bool> running_{ false };
    std::vector<std::thread> threads_;
    ThreadSafeQueue<FileEvent> events_;
    std::vector<std::wstring> directories_;
};