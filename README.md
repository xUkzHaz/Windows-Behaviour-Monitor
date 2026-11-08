# Windows Behaviour Monitor

A real-time Windows telemetry and system monitoring tool built with C++20, Dear ImGui, and DirectX 11. 

This application provides deep visibility into system activities by leveraging native Windows APIs, including Event Tracing for Windows (ETW) for kernel-level monitoring.

## ✨ Features

- **ETW Kernel Monitoring:** Real-time process, thread, and image load tracking via the NT Kernel Logger.
- **Process Explorer:** Detailed CPU usage, memory, and handle counts with the ability to kill, suspend, or resume processes.
- **Network Monitor:** Live TCP/UDP connections mapped directly to their owning Process IDs (PIDs).
- **Registry Monitor:** Watches for modifications in sensitive autorun keys (`Run`, `RunOnce`, `Services`).
- **File Activity Monitor:** Watches for file creation, modification, and deletion in `%USERPROFILE%` and `%TEMP%`.
- **Alert Engine:** Flags suspicious behavior, such as executables launching from temporary directories or unauthorized registry changes.
- **Professional UI:** A fully dockable, edge-to-edge dark mode interface with custom theming.

## 📁 Project Structure

```text
WindowsBehaviourMonitor/
├── CMakeLists.txt
├── README.md
└── src/
    ├── main.cpp
    ├── Application.h / .cpp
    ├── core/
    │   ├── Window.h / .cpp           # DirectX 11 Window Management
    │   ├── ImGuiLayer.h / .cpp       # ImGui Initialization and Rendering
    │   ├── Utils.h / .cpp            # Windows API Helpers
    │   └── ThreadSafeQueue.h         # Lock-free(ish) queue for multi-threading
    ├── data/
    │   └── Models.h                  # Shared Data Structs (ProcessInfo, Alert, etc.)
    ├── monitors/
    │   ├── IMonitor.h                # Base Interface
    │   ├── SystemMonitor.h / .cpp    # CPU/RAM Telemetry
    │   ├── ProcessMonitor.h / .cpp   # Process Enumeration & Control
    │   ├── EtwMonitor.h / .cpp       # Kernel-level Event Tracing
    │   ├── RegistryMonitor.h / .cpp  # Autorun Registry watching
    │   ├── FileMonitor.h / .cpp      # Directory watching
    │   └── NetworkMonitor.h / .cpp   # TCP/UDP Connection tracking
    └── ui/
        ├── Theme.h / .cpp            # Professional Dark Theme Styling
        ├── UIManager.h / .cpp        # Docking Layout & Main Loop
        └── panels/                   # Individual UI Windows
            ├── DashboardPanel.h / .cpp
            ├── ProcessPanel.h / .cpp
            ├── EventLogPanel.h / .cpp
            ├── NetworkPanel.h / .cpp
            ├── RegistryPanel.h / .cpp
            ├── FilePanel.h / .cpp
            ├── AlertsPanel.h / .cpp
            └── SettingsPanel.h / .cpp
