#include "NetworkMonitor.h"
#include "core/Utils.h"
#include <windows.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <chrono>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

bool NetworkMonitor::Start()
{
    if (running_) return true;
    running_ = true;
    thread_ = std::thread(&NetworkMonitor::Loop, this);
    return true;
}

void NetworkMonitor::Stop()
{
    running_ = false;
    if (thread_.joinable()) thread_.join();
}

std::vector<NetworkConnection> NetworkMonitor::GetConnections()
{
    std::lock_guard lock(mutex_);
    return connections_;
}

void NetworkMonitor::Loop()
{
    while (running_)
    {
        Refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

void NetworkMonitor::Refresh()
{
    std::vector<NetworkConnection> result;

    auto addTcp = [&](ULONG family) {
        DWORD size = 0;
        if (GetExtendedTcpTable(nullptr, &size, FALSE, family, TCP_TABLE_OWNER_PID_ALL, 0) != ERROR_INSUFFICIENT_BUFFER)
            return;
        std::vector<BYTE> buf(size);
        auto* table = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID>(buf.data());
        if (GetExtendedTcpTable(table, &size, FALSE, family, TCP_TABLE_OWNER_PID_ALL, 0) != NO_ERROR)
            return;

        for (DWORD i = 0; i < table->dwNumEntries; ++i)
        {
            auto& row = table->table[i];
            NetworkConnection c;
            c.protocol = "TCP";
            c.localAddress = Utils::AddressToString(row.dwLocalAddr);
            c.localPort = Utils::PortToString((USHORT)row.dwLocalPort);
            c.remoteAddress = Utils::AddressToString(row.dwRemoteAddr);
            c.remotePort = Utils::PortToString((USHORT)row.dwRemotePort);
            c.pid = row.dwOwningPid;
            c.processName = Utils::GetProcessName(c.pid);

            switch (row.dwState)
            {
            case MIB_TCP_STATE_ESTAB: c.state = "ESTABLISHED"; break;
            case MIB_TCP_STATE_LISTEN: c.state = "LISTENING"; break;
            case MIB_TCP_STATE_TIME_WAIT: c.state = "TIME_WAIT"; break;
            case MIB_TCP_STATE_SYN_SENT: c.state = "SYN_SENT"; break;
            case MIB_TCP_STATE_SYN_RCVD: c.state = "SYN_RECV"; break;
            case MIB_TCP_STATE_FIN_WAIT1: c.state = "FIN_WAIT1"; break;
            case MIB_TCP_STATE_FIN_WAIT2: c.state = "FIN_WAIT2"; break;
            case MIB_TCP_STATE_CLOSE_WAIT: c.state = "CLOSE_WAIT"; break;
            case MIB_TCP_STATE_CLOSING: c.state = "CLOSING"; break;
            case MIB_TCP_STATE_LAST_ACK: c.state = "LAST_ACK"; break;
            case MIB_TCP_STATE_CLOSED: c.state = "CLOSED"; break;
            default: c.state = "UNKNOWN"; break;
            }
            result.push_back(std::move(c));
        }
        };

    addTcp(AF_INET);
    addTcp(AF_INET6);

    // UDP
    auto addUdp = [&](ULONG family) {
        DWORD size = 0;
        if (GetExtendedUdpTable(nullptr, &size, FALSE, family, UDP_TABLE_OWNER_PID, 0) != ERROR_INSUFFICIENT_BUFFER)
            return;
        std::vector<BYTE> buf(size);
        auto* table = reinterpret_cast<PMIB_UDPTABLE_OWNER_PID>(buf.data());
        if (GetExtendedUdpTable(table, &size, FALSE, family, UDP_TABLE_OWNER_PID, 0) != NO_ERROR)
            return;

        for (DWORD i = 0; i < table->dwNumEntries; ++i)
        {
            auto& row = table->table[i];
            NetworkConnection c;
            c.protocol = "UDP";
            c.localAddress = Utils::AddressToString(row.dwLocalAddr);
            c.localPort = Utils::PortToString((USHORT)row.dwLocalPort);
            c.remoteAddress = "*";
            c.remotePort = "*";
            c.state = "UNCONNECTED";
            c.pid = row.dwOwningPid;
            c.processName = Utils::GetProcessName(c.pid);
            result.push_back(std::move(c));
        }
        };

    addUdp(AF_INET);
    addUdp(AF_INET6);

    std::lock_guard lock(mutex_);
    connections_ = std::move(result);
}