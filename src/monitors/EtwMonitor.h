#pragma once
#include "IMonitor.h"
#include "core/ThreadSafeQueue.h"
#include "data/Models.h"
#include <atomic>
#include <thread>
#include <windows.h>
#include <evntrace.h>
#include <evntcons.h>

class EtwMonitor : public IMonitor
{
public:
    bool Start() override;
    void Stop() override;
    ThreadSafeQueue<EtwEvent>& Events() { return events_; }

private:
    static void WINAPI EventRecordCallback(PEVENT_RECORD record);
    void ProcessEvent(PEVENT_RECORD record);

    std::atomic<bool> running_{ false };
    std::thread thread_;
    TRACEHANDLE sessionHandle_ = 0;
    TRACEHANDLE traceHandle_ = 0;
    ThreadSafeQueue<EtwEvent> events_;
};