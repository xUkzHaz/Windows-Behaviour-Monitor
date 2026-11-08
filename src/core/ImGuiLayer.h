#pragma once
#include <windows.h>
#include "Window.h"

class ImGuiLayer
{
public:
    bool Initialize(Window& window);
    void Shutdown();
    void BeginFrame();
    void EndFrame();
    void Render(Window& window);

private:
    void LoadFonts();
};