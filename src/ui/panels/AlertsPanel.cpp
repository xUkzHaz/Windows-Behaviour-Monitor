#include "AlertsPanel.h"
#include "ui/UIManager.h"
#include "imgui.h"

namespace AlertsPanel
{
    void Draw(UIManager& ui)
    {
        ImGui::Begin("Alerts");

        if (ImGui::Button("Clear All"))
            ui.Alerts().clear();

        ImGui::Separator();

        for (auto& a : ui.Alerts())
        {
            ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            const char* label = "INFO";
            if (a.severity == Alert::Severity::Warning) { color = ImVec4(0.90f, 0.75f, 0.20f, 1.0f); label = "WARN"; }
            if (a.severity == Alert::Severity::Critical) { color = ImVec4(0.85f, 0.25f, 0.25f, 1.0f); label = "CRIT"; }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::Text("[%s]", label);
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::TextUnformatted(a.title.c_str());
            ImGui::Indent();
            ImGui::TextDisabled("%s", a.description.c_str());
            ImGui::Unindent();
            ImGui::Separator();
        }

        if (ui.Alerts().empty())
            ImGui::TextDisabled("No alerts.");

        ImGui::End();
    }
}