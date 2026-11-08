#pragma once
#include "imgui.h"
#include "monitors/SystemMonitor.h"
#include "monitors/ProcessMonitor.h"
#include "monitors/EtwMonitor.h"
#include "monitors/RegistryMonitor.h"
#include "monitors/FileMonitor.h"
#include "monitors/NetworkMonitor.h"
#include <vector>
#include <deque>

class UIManager
{
public:
    void Initialize(SystemMonitor* sys, ProcessMonitor* proc, EtwMonitor* etw,
        RegistryMonitor* reg, FileMonitor* file, NetworkMonitor* net);

    void Draw();

    // Data access for panels
    SystemStats GetSystemStats() const;
    std::vector<ProcessInfo> GetProcesses() const;
    std::vector<NetworkConnection> GetNetworkConnections() const;

    std::deque<EtwEvent>& EtwEvents() { return etwEvents_; }
    std::deque<RegistryEvent>& RegistryEvents() { return registryEvents_; }
    std::deque<FileEvent>& FileEvents() { return fileEvents_; }
    std::deque<Alert>& Alerts() { return alerts_; }

    bool& ShowDashboard() { return showDashboard_; }
    bool& ShowProcesses() { return showProcesses_; }
    bool& ShowEventLog() { return showEventLog_; }
    bool& ShowNetwork() { return showNetwork_; }
    bool& ShowRegistry() { return showRegistry_; }
    bool& ShowFiles() { return showFiles_; }
    bool& ShowAlerts() { return showAlerts_; }
    bool& ShowSettings() { return showSettings_; }

    void RequestQuit() { quitRequested_ = true; }
    bool QuitRequested() const { return quitRequested_; }

private:
    void PullEvents();
    void BuildDockSpace();
    void DrawMenuBar();
    void DrawStatusBar();
    void CheckAlerts(const EtwEvent& ev);
    void CheckRegistryAlert(const RegistryEvent& ev);

    SystemMonitor* sys_ = nullptr;
    ProcessMonitor* proc_ = nullptr;
    EtwMonitor* etw_ = nullptr;
    RegistryMonitor* reg_ = nullptr;
    FileMonitor* file_ = nullptr;
    NetworkMonitor* net_ = nullptr;

    std::deque<EtwEvent> etwEvents_;
    std::deque<RegistryEvent> registryEvents_;
    std::deque<FileEvent> fileEvents_;
    std::deque<Alert> alerts_;

    bool showDashboard_ = true;
    bool showProcesses_ = true;
    bool showEventLog_ = true;
    bool showNetwork_ = true;
    bool showRegistry_ = true;
    bool showFiles_ = true;
    bool showAlerts_ = true;
    bool showSettings_ = true;
    bool quitRequested_ = false;

    bool dockLayoutInitialized_ = false;
};