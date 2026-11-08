#include "FileMonitor.h"
#include "core/Utils.h"
#include <windows.h>

bool FileMonitor::Start()
{
    if (running_) return true;
    running_ = true;

    wchar_t profile[MAX_PATH]{};
    GetEnvironmentVariableW(L"USERPROFILE", profile, MAX_PATH);
    if (profile[0]) directories_.push_back(profile);

    wchar_t temp[MAX_PATH]{};
    GetTempPathW(MAX_PATH, temp);
    if (temp[0]) directories_.push_back(temp);

    for (auto& dir : directories_)
        threads_.emplace_back(&FileMonitor::WatchLoop, this, dir);

    return true;
}

void FileMonitor::Stop()
{
    running_ = false;
    for (auto& t : threads_)
        if (t.joinable()) t.join();
    threads_.clear();
}

void FileMonitor::WatchDirectory(const std::wstring& path)
{
    directories_.push_back(path);
}

void FileMonitor::WatchLoop(std::wstring path)
{
    HANDLE dir = CreateFileW(path.c_str(), FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);

    if (dir == INVALID_HANDLE_VALUE) return;

    alignas(DWORD) BYTE buffer[65536];
    DWORD bytes = 0;

    while (running_)
    {
        if (!ReadDirectoryChangesW(dir, buffer, sizeof(buffer), TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE,
            &bytes, nullptr, nullptr))
            break;

        if (bytes == 0) continue;

        auto* info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
        while (true)
        {
            std::wstring name(info->FileName, info->FileNameLength / sizeof(wchar_t));
            std::wstring full = path + L"\\" + name;

            std::string action;
            switch (info->Action)
            {
            case FILE_ACTION_ADDED: action = "Added"; break;
            case FILE_ACTION_MODIFIED: action = "Modified"; break;
            case FILE_ACTION_REMOVED: action = "Removed"; break;
            case FILE_ACTION_RENAMED_OLD_NAME: action = "Renamed (old)"; break;
            case FILE_ACTION_RENAMED_NEW_NAME: action = "Renamed (new)"; break;
            }

            FileEvent ev;
            ev.path = Utils::WStringToUTF8(full);
            ev.action = action;
            events_.Push(ev);

            if (info->NextEntryOffset == 0) break;
            info = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                reinterpret_cast<BYTE*>(info) + info->NextEntryOffset);
        }
    }

    CloseHandle(dir);
}