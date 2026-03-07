// dear raingui: Renderer Backend for DirectX10
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture backend. Use 'ID3D10ShaderResourceView*' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

// You can copy and use unmodified raingui_impl_* files in your project. See examples/ folder for examples of using this.
// If you are new to Dear RainGui, read documentation from the docs/ folder + read the top of raingui.cpp.
// Read online: https://github.com/ocornut/raingui/tree/master/docs

#pragma once
#include "raingui.h"      // RAINGUI_IMPL_API

struct ID3D10Device;

RAINGUI_IMPL_API bool     RainGui_ImplDX10_Init(ID3D10Device* device);
RAINGUI_IMPL_API void     RainGui_ImplDX10_Shutdown();
RAINGUI_IMPL_API void     RainGui_ImplDX10_NewFrame();
RAINGUI_IMPL_API void     RainGui_ImplDX10_RenderDrawData(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing Dear RainGui state.
RAINGUI_IMPL_API void     RainGui_ImplDX10_InvalidateDeviceObjects();
RAINGUI_IMPL_API bool     RainGui_ImplDX10_CreateDeviceObjects();
