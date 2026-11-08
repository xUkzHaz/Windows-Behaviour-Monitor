#include "Utils.h"
#include <psapi.h>
#include <vector>
#include <iphlpapi.h>
#include <ws2tcpip.h>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

namespace Utils
{
    std::string WStringToUTF8(const std::wstring& wstr)
    {
        if (wstr.empty()) return {};
        int size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string result(size, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), result.data(), size, nullptr, nullptr);
        return result;
    }

    std::wstring UTF8ToWString(const std::string& str)
    {
        if (str.empty()) return {};
        int size = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), nullptr, 0);
        std::wstring result(size, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), result.data(), size);
        return result;
    }

    std::string GetProcessName(DWORD pid)
    {
        if (pid == 0) return "System Idle Process";
        if (pid == 4) return "System";

        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!h) return "<unknown>";

        wchar_t path[MAX_PATH]{};
        DWORD size = MAX_PATH;
        std::string name = "<unknown>";
        if (QueryFullProcessImageNameW(h, 0, path, &size))
        {
            std::wstring full(path);
            size_t pos = full.find_last_of(L"\\/");
            name = WStringToUTF8(pos != std::wstring::npos ? full.substr(pos + 1) : full);
        }
        CloseHandle(h);
        return name;
    }

    std::string GetProcessPath(DWORD pid)
    {
        HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (!h) return {};
        wchar_t path[MAX_PATH]{};
        DWORD size = MAX_PATH;
        std::string result;
        if (QueryFullProcessImageNameW(h, 0, path, &size))
            result = WStringToUTF8(path);
        CloseHandle(h);
        return result;
    }

    std::string GetLastErrorString(DWORD error)
    {
        LPWSTR buffer = nullptr;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&buffer, 0, nullptr);
        std::wstring msg = buffer ? buffer : L"Unknown error";
        LocalFree(buffer);
        return WStringToUTF8(msg);
    }

    std::string FormatSystemTime(const SYSTEMTIME& st)
    {
        char buf[64];
        sprintf_s(buf, "%04d-%02d-%02d %02d:%02d:%02d",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
        return buf;
    }

    std::string FormatFileTime(const FILETIME& ft)
    {
        SYSTEMTIME st{};
        FileTimeToSystemTime(&ft, &st);
        return FormatSystemTime(st);
    }

    std::string AddressToString(DWORD addr)
    {
        in_addr a{};
        a.S_un.S_addr = addr;
        char buf[INET_ADDRSTRLEN]{};
        inet_ntop(AF_INET, &a, buf, sizeof(buf));
        return buf;
    }

    std::string PortToString(USHORT port)
    {
        return std::to_string(ntohs(port));
    }

    bool IsRunningAsAdmin()
    {
        BOOL admin = FALSE;
        PSID group = nullptr;
        SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
        if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
            DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &group))
        {
            CheckTokenMembership(nullptr, group, &admin);
            FreeSid(group);
        }
        return admin == TRUE;
    }
}