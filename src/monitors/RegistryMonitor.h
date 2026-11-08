#pragma once
#include "IMonitor.h"
#include "core/ThreadSafeQueue.h"
#include "data/Models.h"
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <windows.h>

class RegistryMonitor : public IMonitor
{
public:
    bool Start() override;
    void Stop() override;
    ThreadSafeQueue<RegistryEvent>& Events() { return events_; }
    void WatchKey(const std::wstring& key);

private:
    void WatchLoop(std::wstring keyPath);
    std::unordered_map<std::wstring, std::wstring> Snapshot(HKEY root, const std::wstring& subKey);

    std::atomic<bool> running_{ false };
    std::vector<std::thread> threads_;
    std::mutex mutex_;
    ThreadSafeQueue<RegistryEvent> events_;
    std::vector<std::wstring> keys_;
};