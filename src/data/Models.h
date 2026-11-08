#pragma once
#include <string>
#include <chrono>
#include <cstdint>
#include <windows.h>

enum class EventType
{
    ProcessStart,
    ProcessStop,
    ThreadStart,
    ThreadStop,
    ImageLoad,
    RegistryChange,
    FileChange,
    NetworkConnection,
    Alert
};

struct ProcessInfo
{
    DWORD pid = 0;
    DWORD ppid = 0;
    std::string name;
    std::string path;
    DWORD threads = 0;
    DWORD handles = 0;
    SIZE_T workingSet = 0;
    SIZE_T privateBytes = 0;
    double cpuPercent = 0.0;
    ULONGLONG startTime = 0;
    int priority = 0;
    bool isElevated = false;

    // For CPU delta
    ULONGLONG lastKernelTime = 0;
    ULONGLONG lastUserTime = 0;
    ULONGLONG lastTimestamp = 0;
};

struct NetworkConnection
{
    std::string protocol;      // TCP / UDP
    std::string localAddress;
    std::string localPort;
    std::string remoteAddress;
    std::string remotePort;
    std::string state;
    DWORD pid = 0;
    std::string processName;
};

struct RegistryEvent
{
    std::string keyPath;
    std::string valueName;
    std::string eventType;     // Added / Modified / Deleted
    std::string timestamp;
    std::string data;
};

struct FileEvent
{
    std::string path;
    std::string action;        // Added / Modified / Removed / Renamed
    std::string timestamp;
};

struct EtwEvent
{
    EventType type = EventType::ProcessStart;
    DWORD pid = 0;
    DWORD tid = 0;
    DWORD parentPid = 0;
    std::string name;
    std::string detail;
    std::string timestamp;
};

struct Alert
{
    enum class Severity { Info, Warning, Critical };
    Severity severity = Severity::Info;
    std::string title;
    std::string description;
    std::string timestamp;
    bool acknowledged = false;
};

struct SystemStats
{
    double cpuUsage = 0.0;
    double memoryUsage = 0.0;
    DWORD processCount = 0;
    DWORD threadCount = 0;
    ULONGLONG uptimeSeconds = 0;
    std::vector<float> cpuHistory;
    std::vector<float> memoryHistory;
};