#pragma once
#include <windows.h>
#include <d3d11.h>

class Window
{
public:
    bool Initialize(HINSTANCE instance, const wchar_t* title, int width, int height);
    void Shutdown();
    bool ProcessMessages();

    HWND Handle() const { return hwnd_; }
    ID3D11Device* Device() const { return device_; }
    ID3D11DeviceContext* Context() const { return context_; }
    IDXGISwapChain* SwapChain() const { return swapChain_; }
    ID3D11RenderTargetView* RenderTarget() const { return renderTarget_; }

    void CreateRenderTarget();
    void CleanupRenderTarget();
    void Resize(int width, int height);

private:
    static LRESULT WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);

    HWND hwnd_ = nullptr;
    ID3D11Device* device_ = nullptr;
    ID3D11DeviceContext* context_ = nullptr;
    IDXGISwapChain* swapChain_ = nullptr;
    ID3D11RenderTargetView* renderTarget_ = nullptr;
    int width_ = 0;
    int height_ = 0;
};