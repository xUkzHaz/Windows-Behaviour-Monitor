#pragma once
#include "core/Window.h"
#include "core/ImGuiLayer.h"
#include "monitors/SystemMonitor.h"
#include "monitors/ProcessMonitor.h"
#include "monitors/EtwMonitor.h"
#include "monitors/RegistryMonitor.h"
#include "monitors/FileMonitor.h"
#include "monitors/NetworkMonitor.h"
#include "ui/UIManager.h"

class Application
{
public:
    bool Initialize();
    void Run();
    void Shutdown();

private:
    Window window_;
    ImGuiLayer imgui_;
    SystemMonitor systemMonitor_;
    ProcessMonitor processMonitor_;
    EtwMonitor etwMonitor_;
    RegistryMonitor registryMonitor_;
    FileMonitor fileMonitor_;
    NetworkMonitor networkMonitor_;
    UIManager ui_;
};