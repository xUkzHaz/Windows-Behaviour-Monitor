cmake_minimum_required(VERSION 3.20)
project(WindowsBehaviourMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# Dear ImGui – docking branch
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC
    d3d11 dxgi d3dcompiler dwmapi
)

# HERE IS THE WIN32 FLAG (Crucial for the .exe to generate)
add_executable(WindowsBehaviourMonitor WIN32
    src/main.cpp
    src/Application.cpp
    src/core/Window.cpp
    src/core/ImGuiLayer.cpp
    src/core/Utils.cpp
    src/monitors/SystemMonitor.cpp
    src/monitors/ProcessMonitor.cpp
    src/monitors/EtwMonitor.cpp
    src/monitors/RegistryMonitor.cpp
    src/monitors/FileMonitor.cpp
    src/monitors/NetworkMonitor.cpp
    src/ui/Theme.cpp
    src/ui/UIManager.cpp
    src/ui/panels/DashboardPanel.cpp
    src/ui/panels/ProcessPanel.cpp
    src/ui/panels/EventLogPanel.cpp
    src/ui/panels/NetworkPanel.cpp
    src/ui/panels/RegistryPanel.cpp
    src/ui/panels/FilePanel.cpp
    src/ui/panels/AlertsPanel.cpp
    src/ui/panels/SettingsPanel.cpp
)

target_include_directories(WindowsBehaviourMonitor PRIVATE src)

target_compile_definitions(WindowsBehaviourMonitor PRIVATE
    UNICODE _UNICODE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

target_link_libraries(WindowsBehaviourMonitor PRIVATE
    imgui
    psapi
    iphlpapi
    pdh
    advapi32
    shell32
    ole32
    shlwapi
    ws2_32
)

# This ensures Visual Studio puts the .exe exactly where we want it
set_target_properties(WindowsBehaviourMonitor PROPERTIES
    OUTPUT_NAME "WindowsBehaviourMonitor"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release"
)