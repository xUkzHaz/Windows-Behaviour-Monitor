#pragma once
#include <string>
#include <windows.h>

namespace Utils
{
    std::string WStringToUTF8(const std::wstring& wstr);
    std::wstring UTF8ToWString(const std::string& str);
    std::string GetProcessName(DWORD pid);
    std::string GetProcessPath(DWORD pid);
    std::string GetLastErrorString(DWORD error = GetLastError());
    std::string FormatSystemTime(const SYSTEMTIME& st);
    std::string FormatFileTime(const FILETIME& ft);
    std::string AddressToString(DWORD addr);
    std::string PortToString(USHORT port);
    bool IsRunningAsAdmin();
}