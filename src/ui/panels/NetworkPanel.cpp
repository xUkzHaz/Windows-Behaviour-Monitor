#include "NetworkPanel.h"
#include "ui/UIManager.h"
#include "imgui.h"

namespace NetworkPanel
{
    void Draw(UIManager& ui)
    {
        ImGui::Begin("Network");
        auto connections = ui.GetNetworkConnections();

        if (ImGui::BeginTable("##netTable", 6,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("Proto", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("Local", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Remote", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Process", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for (auto& c : connections)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", c.protocol.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s:%s", c.localAddress.c_str(), c.localPort.c_str());
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s:%s", c.remoteAddress.c_str(), c.remotePort.c_str());
                ImGui::TableSetColumnIndex(3);
                ImGui::TextColored(
                    c.state == "LISTENING" ? ImVec4(0.55f, 0.85f, 0.55f, 1.0f) :
                    c.state == "ESTABLISHED" ? ImVec4(0.0f, 0.75f, 0.75f, 1.0f) :
                    ImGui::GetStyleColorVec4(ImGuiCol_Text), "%s", c.state.c_str());
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%lu", c.pid);
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("%s", c.processName.c_str());
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
}