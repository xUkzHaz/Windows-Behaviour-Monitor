#include "FilePanel.h"
#include "ui/UIManager.h"
#include "imgui.h"

namespace FilePanel
{
    void Draw(UIManager& ui)
    {
        ImGui::Begin("File Activity");
        ImGui::TextDisabled("Watching USERPROFILE and TEMP directories.");
        ImGui::Separator();

        if (ImGui::BeginTable("##fileTable", 3,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableHeadersRow();

            for (auto& ev : ui.FileEvents())
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextColored(
                    ev.action == "Added" ? ImVec4(0.55f, 0.85f, 0.55f, 1.0f) :
                    ev.action == "Removed" ? ImVec4(0.85f, 0.55f, 0.55f, 1.0f) :
                    ImVec4(0.90f, 0.75f, 0.20f, 1.0f), "%s", ev.action.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", ev.path.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::TextDisabled("%s", ev.timestamp.c_str());
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
}