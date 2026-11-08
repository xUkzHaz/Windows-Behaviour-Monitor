#include "Application.h"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    Application app;
    if (!app.Initialize())
        return 1;
    app.Run();
    app.Shutdown();
    return 0;
}