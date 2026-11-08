#include "ProcessMonitor.h"
#include "core/Utils.h"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <chrono>

#pragma comment(lib, "psapi.lib")

bool ProcessMonitor::Start()
{
    if (running_) return true;
    running_ = true;
    thread_ = std::thread(&ProcessMonitor::Loop, this);
    return true;
}

void ProcessMonitor::Stop()
{
    running_ = false;
    if (thread_.joinable()) thread_.join();
}

std::vector<ProcessInfo> ProcessMonitor::GetProcesses()
{
    std::lock_guard lock(mutex_);
    std::vector<ProcessInfo> result;
    result.reserve(processes_.size());
    for (auto& [pid, info] : processes_)
        result.push_back(info);
    return result;
}

void ProcessMonitor::Loop()
{
    while (running_)
    {
        Refresh();
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

static ULONGLONG FileTimeToULL(const FILETIME& ft)
{
    ULARGE_INTEGER u; u.LowPart = ft.dwLowDateTime; u.HighPart = ft.dwHighDateTime; return u.QuadPart;
}

void ProcessMonitor::Refresh()
{
    std::unordered_map<DWORD, ProcessInfo> next;

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe{};
    pe.dwSize = sizeof(pe);
    if (!Process32FirstW(snap, &pe))
    {
        CloseHandle(snap);
        return;
    }

    ULONGLONG now = GetTickCount64();

    do
    {
        ProcessInfo info;
        info.pid = pe.th32ProcessID;
        info.ppid = pe.th32ParentProcessID;
        info.name = Utils::WStringToUTF8(pe.szExeFile);
        info.threads = pe.cntThreads;
        info.priority = pe.pcPriClassBase;
        info.path = Utils::GetProcessPath(info.pid);

        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, info.pid);
        if (h)
        {
            PROCESS_MEMORY_COUNTERS_EX pmc{};
            if (GetProcessMemoryInfo(h, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
            {
                info.workingSet = pmc.WorkingSetSize;
                info.privateBytes = pmc.PrivateUsage;
            }

            FILETIME creation, exit, kernel, user;
            if (GetProcessTimes(h, &creation, &exit, &kernel, &user))
            {
                info.startTime = FileTimeToULL(creation);
                info.lastKernelTime = FileTimeToULL(kernel);
                info.lastUserTime = FileTimeToULL(user);
                info.lastTimestamp = now;
            }

            DWORD handleCount = 0;
            if (GetProcessHandleCount(h, &handleCount))
                info.handles = handleCount;

            CloseHandle(h);
        }

        // CPU delta
        auto it = processes_.find(info.pid);
        if (it != processes_.end())
        {
            ULONGLONG kernelDelta = info.lastKernelTime - it->second.lastKernelTime;
            ULONGLONG userDelta = info.lastUserTime - it->second.lastUserTime;
            ULONGLONG timeDelta = info.lastTimestamp - it->second.lastTimestamp;
            if (timeDelta > 0)
            {
                
                info.cpuPercent = 100.0 * (double)(kernelDelta + userDelta) / (double)(timeDelta * 10000);
            }
        }

        next[info.pid] = std::move(info);
    } while (Process32NextW(snap, &pe));

    CloseHandle(snap);

    std::lock_guard lock(mutex_);
    processes_ = std::move(next);
    lastRefresh_ = now;
}

bool ProcessMonitor::KillProcess(DWORD pid)
{
    HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!h) return false;
    bool ok = TerminateProcess(h, 1) == TRUE;
    CloseHandle(h);
    return ok;
}

bool ProcessMonitor::SuspendProcess(DWORD pid)
{
    using NtSuspendProcess = LONG(NTAPI*)(HANDLE);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto fn = reinterpret_cast<NtSuspendProcess>(GetProcAddress(ntdll, "NtSuspendProcess"));
    if (!fn) return false;
    HANDLE h = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid);
    if (!h) return false;
    bool ok = fn(h) == 0;
    CloseHandle(h);
    return ok;
}

bool ProcessMonitor::ResumeProcess(DWORD pid)
{
    using NtResumeProcess = LONG(NTAPI*)(HANDLE);
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    auto fn = reinterpret_cast<NtResumeProcess>(GetProcAddress(ntdll, "NtResumeProcess"));
    if (!fn) return false;
    HANDLE h = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid);
    if (!h) return false;
    bool ok = fn(h) == 0;
    CloseHandle(h);
    return ok;
}