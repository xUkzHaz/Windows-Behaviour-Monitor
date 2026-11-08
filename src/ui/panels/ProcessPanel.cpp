#include "ui/panels/ProcessPanel.h"
#include "ui/UIManager.h"
#include "imgui.h"
#include <algorithm>
#include <string>

namespace ProcessPanel
{
    static char filter[128] = "";
    static int sortColumn = 0;
    static bool sortAsc = true;

    void Draw(UIManager& ui)
    {
        ImGui::Begin("Process Explorer");

        ImGui::SetNextItemWidth(250);
        ImGui::InputTextWithHint("##filter", "Filter by name or PID...", filter, sizeof(filter));
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) {}

        auto processes = ui.GetProcesses();

        std::vector<ProcessInfo> filtered;
        std::string f = filter;
        std::transform(f.begin(), f.end(), f.begin(), ::tolower);
        for (auto& p : processes)
        {
            std::string name = p.name;
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            if (f.empty() || name.find(f) != std::string::npos ||
                std::to_string(p.pid).find(f) != std::string::npos)
                filtered.push_back(p);
        }

        if (ImGui::BeginTable("##procTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("CPU %", ImGuiTableColumnFlags_WidthFixed, 70);
            ImGui::TableSetupColumn("Working Set", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableSetupColumn("Threads", ImGuiTableColumnFlags_WidthFixed, 70);
            ImGui::TableSetupColumn("Handles", ImGuiTableColumnFlags_WidthFixed, 70);
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto& p : filtered)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%lu", p.pid);
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", p.name.c_str());
                ImGui::TableSetColumnIndex(2); ImGui::Text("%.1f", p.cpuPercent);
                ImGui::TableSetColumnIndex(3); ImGui::Text("%.1f MB", p.workingSet / (1024.0 * 1024.0));
                ImGui::TableSetColumnIndex(4); ImGui::Text("%lu", p.threads);
                ImGui::TableSetColumnIndex(5); ImGui::Text("%lu", p.handles);
                ImGui::TableSetColumnIndex(6); ImGui::TextDisabled("%s", p.path.c_str());
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }
}