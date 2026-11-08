#include "RegistryPanel.h"
#include "ui/UIManager.h"
#include "imgui.h"

namespace RegistryPanel
{
    void Draw(UIManager& ui)
    {
        ImGui::Begin("Registry");
        ImGui::TextDisabled("Monitored keys:");
        ImGui::BulletText("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
        ImGui::BulletText("HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce");
        ImGui::BulletText("HKLM\\SYSTEM\\CurrentControlSet\\Services");
        ImGui::Separator();

        if (ImGui::BeginTable("##regTable", 4,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Event", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Data", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto& ev : ui.RegistryEvents())
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", ev.keyPath.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", ev.valueName.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::TextColored(ImVec4(0.90f, 0.75f, 0.20f, 1.0f), "%s", ev.eventType.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::TextDisabled("%s", ev.data.c_str());
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
}