#include "UIManager.h"
#include "Theme.h"
#include "core/Utils.h"
#include "imgui_internal.h"
#include "ui/panels/DashboardPanel.h"
#include "ui/panels/ProcessPanel.h"
#include "ui/panels/EventLogPanel.h"
#include "ui/panels/NetworkPanel.h"
#include "ui/panels/RegistryPanel.h"
#include "ui/panels/FilePanel.h"
#include "ui/panels/AlertsPanel.h"
#include "ui/panels/SettingsPanel.h"
#include <algorithm>

void UIManager::Initialize(SystemMonitor* sys, ProcessMonitor* proc, EtwMonitor* etw,
    RegistryMonitor* reg, FileMonitor* file, NetworkMonitor* net)
{
    sys_ = sys;
    proc_ = proc;
    etw_ = etw;
    reg_ = reg;
    file_ = file;
    net_ = net;

    Theme::ApplyDarkTheme();
    Theme::ApplyProfessionalStyle();
}

SystemStats UIManager::GetSystemStats() const { return sys_->GetStats(); }
std::vector<ProcessInfo> UIManager::GetProcesses() const { return proc_->GetProcesses(); }
std::vector<NetworkConnection> UIManager::GetNetworkConnections() const { return net_->GetConnections(); }

void UIManager::Draw()
{
    PullEvents();

    if (!dockLayoutInitialized_)
    {
        ImGui::DockBuilderRemoveNode(ImGui::GetID("WBM_DockSpace"));
        ImGui::DockBuilderAddNode(ImGui::GetID("WBM_DockSpace"), ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(ImGui::GetID("WBM_DockSpace"), ImGui::GetMainViewport()->WorkSize);

        ImGuiID dockMain = ImGui::GetID("WBM_DockSpace");
        ImGuiID dockLeft, dockRight, dockBottom, dockCenter;

        ImGui::DockBuilderSplitNode(dockMain, ImGuiDir_Left, 0.45f, &dockLeft, &dockCenter);

        ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, 0.35f, &dockRight, &dockCenter);

        ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, 0.35f, &dockBottom, &dockCenter);

        ImGui::DockBuilderDockWindow("Process Explorer", dockLeft);

        ImGui::DockBuilderDockWindow("Network", dockRight);
        ImGui::DockBuilderDockWindow("Registry", dockRight);
        ImGui::DockBuilderDockWindow("Alerts", dockRight);

        ImGui::DockBuilderDockWindow("Event Log", dockBottom);
        ImGui::DockBuilderDockWindow("File Activity", dockBottom);
        ImGui::DockBuilderDockWindow("Settings", dockBottom);

        ImGui::DockBuilderDockWindow("Dashboard", dockCenter);

        ImGui::DockBuilderFinish(ImGui::GetID("WBM_DockSpace"));
        dockLayoutInitialized_ = true;
    }

    BuildDockSpace();
    DrawMenuBar();
    DrawStatusBar();

    if (showDashboard_) DashboardPanel::Draw(*this);
    if (showProcesses_) ProcessPanel::Draw(*this);
    if (showEventLog_) EventLogPanel::Draw(*this);
    if (showNetwork_) NetworkPanel::Draw(*this);
    if (showRegistry_) RegistryPanel::Draw(*this);
    if (showFiles_) FilePanel::Draw(*this);
    if (showAlerts_) AlertsPanel::Draw(*this);
    if (showSettings_) SettingsPanel::Draw(*this);
}

void UIManager::BuildDockSpace()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Cover the entire window
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoBackground; // <--- Makes the background transparent

    // Remove padding and borders
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("WBM_DockSpaceHost", nullptr, flags);
    ImGui::PopStyleVar(3);

    // The actual DockSpace
    ImGui::DockSpace(ImGui::GetID("WBM_DockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    // Menu bar inside the dock host
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", "Alt+F4"))
                RequestQuit();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Dashboard", nullptr, &showDashboard_);
            ImGui::MenuItem("Process Explorer", nullptr, &showProcesses_);
            ImGui::MenuItem("Event Log", nullptr, &showEventLog_);
            ImGui::MenuItem("Network", nullptr, &showNetwork_);
            ImGui::MenuItem("Registry", nullptr, &showRegistry_);
            ImGui::MenuItem("File Activity", nullptr, &showFiles_);
            ImGui::MenuItem("Alerts", nullptr, &showAlerts_);
            ImGui::MenuItem("Settings", nullptr, &showSettings_);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void UIManager::DrawMenuBar() {}

void UIManager::DrawStatusBar()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - 24.0f));
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 24.0f));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;

    ImGui::Begin("##StatusBar", nullptr, flags);
    ImGui::TextUnformatted("  Windows Behaviour Monitor");
    ImGui::SameLine(ImGui::GetWindowWidth() - 220.0f);
    ImGui::TextColored(ImVec4(0.0f, 0.75f, 0.75f, 1.0f), "● LIVE");
    ImGui::SameLine();
    ImGui::TextDisabled("Admin: %s", Utils::IsRunningAsAdmin() ? "Yes" : "No");
    ImGui::End();
}

void UIManager::PullEvents()
{
    auto drainEtw = etw_->Events().Drain();
    for (auto& ev : drainEtw)
    {
        CheckAlerts(ev);
        etwEvents_.push_back(std::move(ev));
        if (etwEvents_.size() > 2000) etwEvents_.pop_front();
    }

    auto drainReg = reg_->Events().Drain();
    for (auto& ev : drainReg)
    {
        CheckRegistryAlert(ev);
        registryEvents_.push_back(std::move(ev));
        if (registryEvents_.size() > 1000) registryEvents_.pop_front();
    }

    auto drainFile = file_->Events().Drain();
    for (auto& ev : drainFile)
    {
        fileEvents_.push_back(std::move(ev));
        if (fileEvents_.size() > 2000) fileEvents_.pop_front();
    }
}

void UIManager::CheckAlerts(const EtwEvent& ev)
{
    if (ev.type != EventType::ProcessStart) return;

    std::string lower = ev.name;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    bool suspicious = false;
    std::string reason;

    if (lower.find("\\temp\\") != std::string::npos ||
        lower.find("\\appdata\\local\\temp") != std::string::npos)
    {
        suspicious = true;
        reason = "Executable launched from a temporary directory.";
    }

    if (lower.find("powershell") != std::string::npos ||
        lower.find("cmd.exe") != std::string::npos ||
        lower.find("wscript") != std::string::npos ||
        lower.find("cscript") != std::string::npos)
    {
        // Only alert if parent is a suspicious process? Keep simple.
        suspicious = true;
        reason = "Script interpreter or command shell started.";
    }

    if (suspicious)
    {
        Alert a;
        a.severity = Alert::Severity::Warning;
        a.title = "Suspicious process start";
        a.description = ev.name + " (PID " + std::to_string(ev.pid) + "): " + reason;
        alerts_.push_back(std::move(a));
        if (alerts_.size() > 500) alerts_.pop_front();
    }
}

void UIManager::CheckRegistryAlert(const RegistryEvent& ev)
{
    std::string lowerKey = ev.keyPath;
    std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);

    if (lowerKey.find("\\run") != std::string::npos ||
        lowerKey.find("\\runonce") != std::string::npos)
    {
        Alert a;
        a.severity = Alert::Severity::Critical;
        a.title = "Autorun registry modification";
        a.description = ev.keyPath + "\\" + ev.valueName + " was " + ev.eventType;
        alerts_.push_back(std::move(a));
        if (alerts_.size() > 500) alerts_.pop_front();
    }
}