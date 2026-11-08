#include "EtwMonitor.h"
#include "core/Utils.h"
#include <windows.h>
#include <evntrace.h>
#include <evntcons.h>
#include <chrono>

#pragma comment(lib, "advapi32.lib")

namespace
{
    const GUID ProcessGuid = { 0x3d6fa8d0, 0xfe05, 0x11d0,{ 0x9d, 0xda, 0x00, 0xc0, 0x4f, 0xd7, 0xba, 0x7c } };
    const GUID ThreadGuid = { 0x3d6fa8d1, 0xfe05, 0x11d0,{ 0x9d, 0xda, 0x00, 0xc0, 0x4f, 0xd7, 0xba, 0x7c } };
    const GUID ImageGuid = { 0x2cb15d1d, 0x5fc1, 0x11d2,{ 0xab, 0xe1, 0x00, 0xa0, 0xc9, 0x11, 0xf5, 0x18 } };

    struct ProcessTraceInfo
    {
        ULONG ProcessId;
        ULONG ParentId;
        ULONG SessionId;
        ULONG ExitStatus;
        CHAR ImageFileName[15];
    };

    struct ThreadTraceInfo
    {
        ULONG ThreadId;
        ULONG ProcessId;
    };

    struct ImageTraceInfo
    {
        PVOID ImageBase;
        ULONG ImageSize;
        ULONG ProcessId;
        CHAR ImageFileName[1];
    };

    EtwMonitor* g_monitor = nullptr;
}

bool EtwMonitor::Start()
{
    if (running_) return true;
    g_monitor = this;

    // Allocate a buffer large enough for the struct + the logger name string
    std::vector<BYTE> buffer(sizeof(EVENT_TRACE_PROPERTIES) + 1024, 0);
    auto* props = reinterpret_cast<PEVENT_TRACE_PROPERTIES>(buffer.data());

    props->Wnode.BufferSize = (ULONG)buffer.size();
    props->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
    props->Wnode.ClientContext = 1;
    props->LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
    props->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);

    // Stop existing session if it exists
    ControlTraceW(0, KERNEL_LOGGER_NAMEW, props, EVENT_TRACE_CONTROL_STOP);

    // Reset buffer for the new session
    std::fill(buffer.begin(), buffer.end(), 0);
    props->Wnode.BufferSize = (ULONG)buffer.size();
    props->Wnode.Flags = WNODE_FLAG_TRACED_GUID;
    props->Wnode.ClientContext = 1;
    props->LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
    props->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);

    props->BufferSize = 64;
    props->MinimumBuffers = 2;
    props->MaximumBuffers = 8;
    props->FlushTimer = 1;
    props->EnableFlags = EVENT_TRACE_FLAG_PROCESS | EVENT_TRACE_FLAG_THREAD | EVENT_TRACE_FLAG_IMAGE_LOAD;

    if (StartTraceW(&sessionHandle_, KERNEL_LOGGER_NAMEW, props) != ERROR_SUCCESS)
        return false;

    EVENT_TRACE_LOGFILEW logfile{};
    logfile.LoggerName = const_cast<LPWSTR>(KERNEL_LOGGER_NAMEW);
    logfile.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD;
    logfile.EventRecordCallback = &EtwMonitor::EventRecordCallback;

    traceHandle_ = OpenTraceW(&logfile);
    if (traceHandle_ == INVALID_PROCESSTRACE_HANDLE)
    {
        ControlTraceW(sessionHandle_, nullptr, props, EVENT_TRACE_CONTROL_STOP);
        return false;
    }

    running_ = true;
    thread_ = std::thread([this]() {
        ProcessTrace(&traceHandle_, 1, nullptr, nullptr);
        });

    return true;
}

void EtwMonitor::Stop()
{
    running_ = false;
    if (traceHandle_ != INVALID_PROCESSTRACE_HANDLE)
    {
        CloseTrace(traceHandle_);
        traceHandle_ = 0;
    }
    if (sessionHandle_)
    {
        std::vector<BYTE> buffer(sizeof(EVENT_TRACE_PROPERTIES) + 1024, 0);
        auto* props = reinterpret_cast<PEVENT_TRACE_PROPERTIES>(buffer.data());
        props->Wnode.BufferSize = (ULONG)buffer.size();
        props->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);

        ControlTraceW(sessionHandle_, KERNEL_LOGGER_NAMEW, props, EVENT_TRACE_CONTROL_STOP);
        sessionHandle_ = 0;
    }
    if (thread_.joinable()) thread_.join();
}

void WINAPI EtwMonitor::EventRecordCallback(PEVENT_RECORD record)
{
    if (g_monitor) g_monitor->ProcessEvent(record);
}

void EtwMonitor::ProcessEvent(PEVENT_RECORD record)
{
    if (!record) return;

    EtwEvent ev;
    ev.pid = record->EventHeader.ProcessId;
    ev.tid = record->EventHeader.ThreadId;
    ev.timestamp = Utils::FormatSystemTime([&] {
        FILETIME ft;
        ft.dwLowDateTime = record->EventHeader.TimeStamp.LowPart;
        ft.dwHighDateTime = record->EventHeader.TimeStamp.HighPart;
        SYSTEMTIME st{}; FileTimeToSystemTime(&ft, &st); return st;
        }());

    const GUID& guid = record->EventHeader.ProviderId;

    if (IsEqualGUID(guid, ProcessGuid))
    {
        
        if (record->UserDataLength >= 20)
        {
            
            struct ProcessPayload {
                ULONG ProcessId;
                ULONG ParentId;
                ULONG SessionId;
                ULONG ExitStatus;
                CHAR ImageFileName[15]; 
            };

            auto* info = reinterpret_cast<ProcessPayload*>(record->UserData);
            ev.type = (record->EventHeader.EventDescriptor.Opcode == EVENT_TRACE_TYPE_START)
                ? EventType::ProcessStart : EventType::ProcessStop;
            ev.pid = info->ProcessId;
            ev.parentPid = info->ParentId;

            
            ev.name = std::string(info->ImageFileName, strnlen(info->ImageFileName, 15));

            events_.Push(ev);
        }
    }
    else if (IsEqualGUID(guid, ThreadGuid))
    {
        auto* info = reinterpret_cast<ThreadTraceInfo*>(record->UserData);
        if (!info) return;
        ev.type = (record->EventHeader.EventDescriptor.Opcode == EVENT_TRACE_TYPE_START)
            ? EventType::ThreadStart : EventType::ThreadStop;
        ev.pid = info->ProcessId;
        ev.tid = info->ThreadId;
        events_.Push(ev);
    }
    else if (IsEqualGUID(guid, ImageGuid))
    {
        auto* info = reinterpret_cast<ImageTraceInfo*>(record->UserData);
        if (!info) return;
        ev.type = EventType::ImageLoad;
        ev.pid = info->ProcessId;
        ev.name = info->ImageFileName;
        events_.Push(ev);
    }
}