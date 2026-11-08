#include "EventLogPanel.h"
#include "ui/UIManager.h"
#include "imgui.h"

namespace EventLogPanel
{
    static bool showProcess = true;
    static bool showThread = true;
    static bool showImage = true;
    static bool showRegistry = true;
    static bool showFile = true;

    void Draw(UIManager& ui)
    {
        ImGui::Begin("Event Log");

        ImGui::Checkbox("Process", &showProcess); ImGui::SameLine();
        ImGui::Checkbox("Thread", &showThread); ImGui::SameLine();
        ImGui::Checkbox("Image", &showImage); ImGui::SameLine();
        ImGui::Checkbox("Registry", &showRegistry); ImGui::SameLine();
        ImGui::Checkbox("File", &showFile);

        ImGui::Separator();

        if (ImGui::BeginTable("##eventTable", 4,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 140);
            ImGui::TableSetupColumn("Details", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            for (auto& ev : ui.EtwEvents())
            {
                bool visible = false;
                const char* type = "";
                ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Text);

                switch (ev.type)
                {
                case EventType::ProcessStart:
                    visible = showProcess; type = "Process +"; color = ImVec4(0.55f, 0.85f, 0.55f, 1.0f); break;
                case EventType::ProcessStop:
                    visible = showProcess; type = "Process -"; color = ImVec4(0.85f, 0.55f, 0.55f, 1.0f); break;
                case EventType::ThreadStart:
                    visible = showThread; type = "Thread +"; break;
                case EventType::ThreadStop:
                    visible = showThread; type = "Thread -"; break;
                case EventType::ImageLoad:
                    visible = showImage; type = "Image"; break;
                default: break;
                }

                if (!visible) continue;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextDisabled("%s", ev.timestamp.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::TextColored(color, "%s", type);
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("PID %lu", ev.pid);
                ImGui::TableSetColumnIndex(3);
                if (ev.type == EventType::ProcessStart || ev.type == EventType::ProcessStop)
                    ImGui::Text("%s (parent %lu)", ev.name.c_str(), ev.parentPid);
                else if (ev.type == EventType::ImageLoad)
                    ImGui::Text("%s", ev.name.c_str());
                else
                    ImGui::Text("TID %lu", ev.tid);
            }

            if (showRegistry)
            {
                for (auto& ev : ui.RegistryEvents())
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextDisabled("%s", ev.timestamp.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(ImVec4(0.90f, 0.75f, 0.20f, 1.0f), "Registry");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", ev.keyPath.c_str());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%s: %s [%s]", ev.valueName.c_str(), ev.eventType.c_str(), ev.data.c_str());
                }
            }

            if (showFile)
            {
                for (auto& ev : ui.FileEvents())
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextDisabled("");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(ImVec4(0.55f, 0.75f, 0.90f, 1.0f), "File");
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", ev.action.c_str());
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%s", ev.path.c_str());
                }
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
}