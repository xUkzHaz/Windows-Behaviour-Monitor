#include "SystemMonitor.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <chrono>

bool SystemMonitor::Start()
{
    if (running_) return true;
    running_ = true;
    thread_ = std::thread(&SystemMonitor::Loop, this);
    return true;
}

void SystemMonitor::Stop()
{
    running_ = false;
    if (thread_.joinable()) thread_.join();
}

SystemStats SystemMonitor::GetStats()
{
    std::lock_guard lock(mutex_);
    return stats_;
}

void SystemMonitor::Loop()
{
    FILETIME idle, kernel, user;
    GetSystemTimes(&idle, &kernel, &user);

    auto toULL = [](const FILETIME& ft) -> ULONGLONG {
        ULARGE_INTEGER u; u.LowPart = ft.dwLowDateTime; u.HighPart = ft.dwHighDateTime; return u.QuadPart;
        };

    prevIdle_ = toULL(idle);
    prevKernel_ = toULL(kernel);
    prevUser_ = toULL(user);

    while (running_)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        GetSystemTimes(&idle, &kernel, &user);
        ULONGLONG idleNow = toULL(idle);
        ULONGLONG kernelNow = toULL(kernel);
        ULONGLONG userNow = toULL(user);

        ULONGLONG idleDelta = idleNow - prevIdle_;
        ULONGLONG kernelDelta = kernelNow - prevKernel_;
        ULONGLONG userDelta = userNow - prevUser_;
        ULONGLONG total = kernelDelta + userDelta;

        double cpu = total > 0 ? (1.0 - (double)idleDelta / (double)total) * 100.0 : 0.0;

        prevIdle_ = idleNow;
        prevKernel_ = kernelNow;
        prevUser_ = userNow;

        MEMORYSTATUSEX mem{};
        mem.dwLength = sizeof(mem);
        GlobalMemoryStatusEx(&mem);

        DWORD processes[1024]{};
        DWORD needed = 0;
        if (EnumProcesses(processes, sizeof(processes), &needed))
        {
            // count
        }

        // process count via Toolhelp for accuracy
        DWORD procCount = 0;
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snap != INVALID_HANDLE_VALUE)
        {
            PROCESSENTRY32W pe{};
            pe.dwSize = sizeof(pe);
            if (Process32FirstW(snap, &pe))
            {
                do { procCount++; } while (Process32NextW(snap, &pe));
            }
            CloseHandle(snap);
        }

        std::lock_guard lock(mutex_);
        stats_.cpuUsage = cpu;
        stats_.memoryUsage = (double)mem.ullTotalPhys > 0
            ? (double)(mem.ullTotalPhys - mem.ullAvailPhys) / (double)mem.ullTotalPhys * 100.0
            : 0.0;
        stats_.processCount = procCount;
        stats_.threadCount = 0;
        stats_.uptimeSeconds = GetTickCount64() / 1000;

        stats_.cpuHistory.push_back((float)stats_.cpuUsage);
        if (stats_.cpuHistory.size() > 120) stats_.cpuHistory.erase(stats_.cpuHistory.begin());
        stats_.memoryHistory.push_back((float)stats_.memoryUsage);
        if (stats_.memoryHistory.size() > 120) stats_.memoryHistory.erase(stats_.memoryHistory.begin());
    }
}