#include "Window.h"
#include <d3d11.h>
#include <dwmapi.h>
#include <tchar.h>
#include "imgui.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwmapi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

bool Window::Initialize(HINSTANCE instance, const wchar_t* title, int width, int height)
{
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = L"WBMWindowClass";
    RegisterClassExW(&wc);

    hwnd_ = CreateWindowW(wc.lpszClassName, title,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        nullptr, nullptr, instance, this);

    if (!hwnd_) return false;

    // Dark title bar
    BOOL dark = TRUE;
    DwmSetWindowAttribute(hwnd_, 20, &dark, sizeof(dark));

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd_;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags,
        levels, 2, D3D11_SDK_VERSION, &sd, &swapChain_, &device_,
        &featureLevel, &context_);

    if (FAILED(hr)) return false;

    CreateRenderTarget();
    ShowWindow(hwnd_, SW_SHOWDEFAULT);
    UpdateWindow(hwnd_);
    return true;
}

void Window::CreateRenderTarget()
{
    ID3D11Texture2D* backBuffer = nullptr;
    swapChain_->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (backBuffer)
    {
        device_->CreateRenderTargetView(backBuffer, nullptr, &renderTarget_);
        backBuffer->Release();
    }
}

void Window::CleanupRenderTarget()
{
    if (renderTarget_) { renderTarget_->Release(); renderTarget_ = nullptr; }
}

void Window::Resize(int width, int height)
{
    if (!swapChain_ || width == 0 || height == 0) return;
    CleanupRenderTarget();
    swapChain_->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    CreateRenderTarget();
    width_ = width;
    height_ = height;
}

bool Window::ProcessMessages()
{
    MSG msg{};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            return false;
    }
    return true;
}

void Window::Shutdown()
{
    CleanupRenderTarget();
    if (swapChain_) { swapChain_->Release(); swapChain_ = nullptr; }
    if (context_) { context_->Release(); context_ = nullptr; }
    if (device_) { device_->Release(); device_ = nullptr; }
    if (hwnd_) { DestroyWindow(hwnd_); hwnd_ = nullptr; }
    UnregisterClassW(L"WBMWindowClass", GetModuleHandle(nullptr));
}

LRESULT WINAPI Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            auto* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
            if (window)
                window->Resize(LOWORD(lParam), HIWORD(lParam));
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    // Store pointer on first message
    if (msg == WM_CREATE)
    {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}