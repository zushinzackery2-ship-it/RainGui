#pragma once

// Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dwmapi.h>

// D3D11
#include <d3d11.h>
#include <dxgi.h>

// RainGui
#define RAINGUI_DEFINE_MATH_OPERATORS
#include "raingui.h"
#include "raingui_internal.h"
#include "raingui_impl_win32.h"
#include "raingui_impl_dx11.h"
#include "raingui_impl_nvidia.h"
