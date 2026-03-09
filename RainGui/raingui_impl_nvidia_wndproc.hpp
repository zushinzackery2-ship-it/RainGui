#pragma once

#include <windows.h>

#include "raingui_impl_win32.h"

extern RAINGUI_IMPL_API LRESULT RainGui_ImplWin32_WndProcHandler(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
);

namespace
{
    // 只在 NVIDIA 后端编译单元中使用，用于接管原始窗口消息。
    static WNDPROC s_rainGuiNvidiaOrigWndProc = nullptr;

    LRESULT CALLBACK RainGuiNvidiaWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        RainGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

        if (msg == WM_NCDESTROY)
        {
            WNDPROC originalWndProc = s_rainGuiNvidiaOrigWndProc;
            s_rainGuiNvidiaOrigWndProc = nullptr;
            if (originalWndProc)
            {
                SetWindowLongPtrA(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWndProc));
                return CallWindowProcA(originalWndProc, hWnd, msg, wParam, lParam);
            }

            return DefWindowProcA(hWnd, msg, wParam, lParam);
        }

        if (s_rainGuiNvidiaOrigWndProc)
        {
            return CallWindowProcA(s_rainGuiNvidiaOrigWndProc, hWnd, msg, wParam, lParam);
        }

        return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
}

inline bool RainGuiNvidia_InstallWndProcHook(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))
    {
        return false;
    }

    if (s_rainGuiNvidiaOrigWndProc)
    {
        return true;
    }

    SetLastError(0);
    LONG_PTR previousWndProc = SetWindowLongPtrA(
        hWnd,
        GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(RainGuiNvidiaWndProc)
    );

    if (previousWndProc == 0 && GetLastError() != 0)
    {
        return false;
    }

    s_rainGuiNvidiaOrigWndProc = reinterpret_cast<WNDPROC>(previousWndProc);
    return s_rainGuiNvidiaOrigWndProc != nullptr;
}

inline void RainGuiNvidia_RemoveWndProcHook(HWND hWnd)
{
    if (!hWnd || !IsWindow(hWnd))
    {
        s_rainGuiNvidiaOrigWndProc = nullptr;
        return;
    }

    if (!s_rainGuiNvidiaOrigWndProc)
    {
        return;
    }

    SetWindowLongPtrA(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(s_rainGuiNvidiaOrigWndProc));
    s_rainGuiNvidiaOrigWndProc = nullptr;
}
