#include "ImGuiLayer.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <string>
#include <windows.h>

bool ImGuiLayer::Initialize(Window& window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // <--- ADD THIS SINGLE LINE HERE
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    LoadFonts();

    ImGui_ImplWin32_Init(window.Handle());
    ImGui_ImplDX11_Init(window.Device(), window.Context());
    return true;
}

void ImGuiLayer::LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    char winDir[MAX_PATH]{};
    GetWindowsDirectoryA(winDir, MAX_PATH);
    std::string fontPath = std::string(winDir) + "\\Fonts\\segoeui.ttf";
    std::string boldPath = std::string(winDir) + "\\Fonts\\seguisb.ttf";

    ImFontConfig config;
    config.OversampleH = 3;
    config.OversampleV = 1;
    config.PixelSnapH = true;

    if (GetFileAttributesA(fontPath.c_str()) != INVALID_FILE_ATTRIBUTES)
        io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f, &config);

    if (GetFileAttributesA(boldPath.c_str()) != INVALID_FILE_ATTRIBUTES)
        io.Fonts->AddFontFromFileTTF(boldPath.c_str(), 18.0f, &config);

    if (io.Fonts->Fonts.empty())
        io.Fonts->AddFontDefault();
}

void ImGuiLayer::BeginFrame()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::EndFrame()
{
    ImGui::Render();
}

void ImGuiLayer::Render(Window& window)
{
    const float clear[4] = { 0.08f, 0.08f, 0.10f, 1.00f };
    auto* ctx = window.Context();
    auto* rtv = window.RenderTarget();
    ctx->OMSetRenderTargets(1, &rtv, nullptr);
    ctx->ClearRenderTargetView(rtv, clear);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    window.SwapChain()->Present(1, 0);
}

void ImGuiLayer::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}