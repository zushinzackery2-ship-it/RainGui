// dear raingui: Platform Backend for Windows (standard windows API for 32 and 64 bits applications)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:
//  [X] Platform: Clipboard support (for Win32 this is actually part of core dear raingui)
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= RainGuiConfigFlags_NoMouseCursorChange'.
//  [X] Platform: Keyboard arrays indexed using VK_* Virtual Key Codes, e.g. RainGui::IsKeyPressed(VK_SPACE).
//  [X] Platform: Gamepad support. Enabled with 'io.ConfigFlags |= RainGuiConfigFlags_NavEnableGamepad'.

// You can copy and use unmodified raingui_impl_* files in your project. See examples/ folder for examples of using this.
// If you are new to Dear RainGui, read documentation from the docs/ folder + read the top of raingui.cpp.
// Read online: https://github.com/ocornut/raingui/tree/master/docs

#pragma once
#include "raingui.h"      // RAINGUI_IMPL_API

RAINGUI_IMPL_API bool     RainGui_ImplWin32_Init(void* hwnd);
RAINGUI_IMPL_API void     RainGui_ImplWin32_Shutdown();
RAINGUI_IMPL_API void     RainGui_ImplWin32_NewFrame();

// Win32 message handler your application need to call.
// - Intentionally commented out in a '#if 0' block to avoid dragging dependencies on <windows.h> from this helper.
// - You should COPY the line below into your .cpp code to forward declare the function and then you can call it.
#if 0
extern RAINGUI_IMPL_API LRESULT RainGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// DPI-related helpers (optional)
// - Use to enable DPI awareness without having to create an application manifest.
// - Your own app may already do this via a manifest or explicit calls. This is mostly useful for our examples/ apps.
// - In theory we could call simple functions from Windows SDK such as SetProcessDPIAware(), SetProcessDpiAwareness(), etc.
//   but most of the functions provided by Microsoft require Windows 8.1/10+ SDK at compile time and Windows 8/10+ at runtime,
//   neither we want to require the user to have. So we dynamically select and load those functions to avoid dependencies.
RAINGUI_IMPL_API void     RainGui_ImplWin32_EnableDpiAwareness();
RAINGUI_IMPL_API float    RainGui_ImplWin32_GetDpiScaleForHwnd(void* hwnd);       // HWND hwnd
RAINGUI_IMPL_API float    RainGui_ImplWin32_GetDpiScaleForMonitor(void* monitor); // HMONITOR monitor

// Transparency related helpers (optional) [experimental]
// - Use to enable alpha compositing transparency with the desktop.
// - Use together with e.g. clearing your framebuffer with zero-alpha.
RAINGUI_IMPL_API void     RainGui_ImplWin32_EnableAlphaCompositing(void* hwnd);   // HWND hwnd
