#include "DashboardPanel.h"
#include "ui/UIManager.h"
#include "imgui.h"
#include <cstdio>

namespace DashboardPanel
{
    static void StatCard(const char* label, const char* value, const char* sub, ImVec4 color)
    {
        ImGui::BeginChild(label, ImVec2(0, 90), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::TextDisabled("%s", label);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
        ImGui::TextColored(color, "%s", value);
        if (sub && *sub)
            ImGui::TextDisabled("%s", sub);
        ImGui::EndChild();
    }

    void Draw(UIManager& ui)
    {
        ImGui::Begin("Dashboard");
        auto stats = ui.GetSystemStats();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
        float w = (ImGui::GetContentRegionAvail().x - 20.0f) / 3.0f;
        ImGui::BeginGroup();
        ImGui::PushItemWidth(w);
        char buf[64];
        sprintf_s(buf, "%.1f%%", stats.cpuUsage);
        StatCard("CPU Usage", buf, "System-wide", ImVec4(0.0f, 0.75f, 0.75f, 1.0f));
        ImGui::PopItemWidth();
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::BeginGroup();
        sprintf_s(buf, "%.1f%%", stats.memoryUsage);
        StatCard("Memory Usage", buf, "Physical RAM", ImVec4(0.90f, 0.75f, 0.20f, 1.0f));
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::BeginGroup();
        sprintf_s(buf, "%lu", stats.processCount);
        StatCard("Processes", buf, "Running", ImVec4(0.55f, 0.85f, 0.55f, 1.0f));
        ImGui::EndGroup();
        ImGui::PopStyleVar();

        ImGui::Spacing();
        ImGui::SeparatorText("Resource History");

        if (!stats.cpuHistory.empty())
        {
            ImGui::TextDisabled("CPU");
            ImGui::PlotLines("##cpu", stats.cpuHistory.data(), (int)stats.cpuHistory.size(),
                0, nullptr, 0.0f, 100.0f, ImVec2(-1, 70));
        }

        if (!stats.memoryHistory.empty())
        {
            ImGui::TextDisabled("Memory");
            ImGui::PlotLines("##mem", stats.memoryHistory.data(), (int)stats.memoryHistory.size(),
                0, nullptr, 0.0f, 100.0f, ImVec2(-1, 70));
        }

        ImGui::Spacing();
        ULONGLONG up = stats.uptimeSeconds;
        ImGui::TextDisabled("System Uptime: %llu days, %02llu:%02llu:%02llu",
            up / 86400, (up % 86400) / 3600, (up % 3600) / 60, up % 60);

        ImGui::End();
    }
}