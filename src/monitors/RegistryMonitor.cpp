#include "RegistryMonitor.h"
#include "core/Utils.h"
#include <windows.h>
#include <vector>

bool RegistryMonitor::Start()
{
    if (running_) return true;
    running_ = true;
    events_.Push({ "SYSTEM", "Monitor Initialized", "Info", "Just now", "Registry monitoring is active." });

    keys_ = {
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce",
        L"SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run",
        L"SYSTEM\\CurrentControlSet\\Services"
    };
    
    for (const auto& key : keys_)
        threads_.emplace_back(&RegistryMonitor::WatchLoop, this, key);

    return true;
}

void RegistryMonitor::Stop()
{
    running_ = false;
    for (auto& t : threads_)
        if (t.joinable()) t.join();
    threads_.clear();
}

void RegistryMonitor::WatchKey(const std::wstring& key)
{
    std::lock_guard lock(mutex_);
    keys_.push_back(key);
}

void RegistryMonitor::WatchLoop(std::wstring keyPath)
{
    HKEY root = HKEY_LOCAL_MACHINE;
    std::wstring subKey = keyPath;
    if (subKey.rfind(L"HKEY_CURRENT_USER\\", 0) == 0)
    {
        root = HKEY_CURRENT_USER;
        subKey = subKey.substr(18);
    }

    HKEY hKey = nullptr;
    if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_NOTIFY | KEY_READ, &hKey) != ERROR_SUCCESS)
        return;

    auto cache = Snapshot(root, subKey);
    HANDLE event = CreateEventW(nullptr, FALSE, FALSE, nullptr);

    while (running_)
    {
        if (RegNotifyChangeKeyValue(hKey, TRUE,
            REG_NOTIFY_CHANGE_NAME | REG_NOTIFY_CHANGE_ATTRIBUTES |
            REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_SECURITY,
            event, TRUE) != ERROR_SUCCESS)
            break;

        if (WaitForSingleObject(event, 1000) == WAIT_OBJECT_0)
        {
            auto current = Snapshot(root, subKey);
            for (auto& [name, value] : current)
            {
                auto it = cache.find(name);
                if (it == cache.end())
                {
                    events_.Push({ Utils::WStringToUTF8(keyPath), Utils::WStringToUTF8(name),
                        "Added", "", Utils::WStringToUTF8(value) });
                }
                else if (it->second != value)
                {
                    events_.Push({ Utils::WStringToUTF8(keyPath), Utils::WStringToUTF8(name),
                        "Modified", "", Utils::WStringToUTF8(value) });
                }
            }
            for (auto& [name, value] : cache)
            {
                if (current.find(name) == current.end())
                {
                    events_.Push({ Utils::WStringToUTF8(keyPath), Utils::WStringToUTF8(name),
                        "Deleted", "", Utils::WStringToUTF8(value) });
                }
            }
            cache = std::move(current);
        }
    }

    CloseHandle(event);
    RegCloseKey(hKey);
}

std::unordered_map<std::wstring, std::wstring> RegistryMonitor::Snapshot(HKEY root, const std::wstring& subKey)
{
    std::unordered_map<std::wstring, std::wstring> result;
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(root, subKey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return result;

    DWORD index = 0;
    wchar_t valueName[16384];
    DWORD valueNameSize;
    BYTE data[16384];
    DWORD dataSize;
    DWORD type;

    while (true)
    {
        valueNameSize = 16384;
        dataSize = 16384;
        LSTATUS status = RegEnumValueW(hKey, index++, valueName, &valueNameSize, nullptr, &type, data, &dataSize);
        if (status == ERROR_NO_MORE_ITEMS) break;
        if (status != ERROR_SUCCESS) continue;

        std::wstring value;
        if (type == REG_SZ || type == REG_EXPAND_SZ)
            value = reinterpret_cast<wchar_t*>(data);
        else if (type == REG_DWORD && dataSize == 4)
            value = std::to_wstring(*reinterpret_cast<DWORD*>(data));
        else
            continue;

        result[valueName] = value;
    }

    RegCloseKey(hKey);
    return result;
}