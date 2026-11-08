#include "Application.h"

bool Application::Initialize()
{
    if (!window_.Initialize(GetModuleHandle(nullptr), L"Windows Behaviour Monitor", 1600, 900))
        return false;

    if (!imgui_.Initialize(window_))
        return false;

    systemMonitor_.Start();
    processMonitor_.Start();
    etwMonitor_.Start();
    registryMonitor_.Start();
    fileMonitor_.Start();
    networkMonitor_.Start();

    ui_.Initialize(&systemMonitor_, &processMonitor_, &etwMonitor_,
        &registryMonitor_, &fileMonitor_, &networkMonitor_);

    return true;
}

void Application::Run()
{
    bool running = true;
    while (running)
    {
        running = window_.ProcessMessages();
        if (ui_.QuitRequested()) running = false;

        imgui_.BeginFrame();
        ui_.Draw();
        imgui_.EndFrame();
        imgui_.Render(window_);
    }
}

void Application::Shutdown()
{
    etwMonitor_.Stop();
    registryMonitor_.Stop();
    fileMonitor_.Stop();
    networkMonitor_.Stop();
    processMonitor_.Stop();
    systemMonitor_.Stop();

    imgui_.Shutdown();
    window_.Shutdown();
}