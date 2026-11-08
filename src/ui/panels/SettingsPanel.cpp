#include "SettingsPanel.h"
#include "ui/UIManager.h"
#include "imgui.h"

namespace SettingsPanel
{
    void Draw(UIManager& ui)
    {
        ImGui::Begin("Settings");

        ImGui::SeparatorText("Panels");
        ImGui::Checkbox("Dashboard", &ui.ShowDashboard());
        ImGui::Checkbox("Process Explorer", &ui.ShowProcesses());
        ImGui::Checkbox("Event Log", &ui.ShowEventLog());
        ImGui::Checkbox("Network", &ui.ShowNetwork());
        ImGui::Checkbox("Registry", &ui.ShowRegistry());
        ImGui::Checkbox("File Activity", &ui.ShowFiles());
        ImGui::Checkbox("Alerts", &ui.ShowAlerts());

        ImGui::SeparatorText("About");
        ImGui::TextWrapped("Windows Behaviour Monitor");
        ImGui::TextDisabled("Real-time kernel, process, network, registry, and file telemetry.");
        ImGui::TextDisabled("Built with Dear ImGui and DirectX 11.");

        ImGui::End();
    }
}