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
    // 退出阶段可能同时遇到窗口销毁和主动 shutdown，这里用原子交换保证只恢复一次 WndProc。
    static PVOID volatile s_rainGuiNvidiaOrigWndProc = nullptr;
    static PVOID volatile s_rainGuiNvidiaHookedWindow = nullptr;

    LRESULT CALLBACK RainGuiNvidiaWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    WNDPROC RainGuiNvidia_GetOrigWndProc()
    {
        return reinterpret_cast<WNDPROC>(
            InterlockedCompareExchangePointer(&s_rainGuiNvidiaOrigWndProc, nullptr, nullptr)
        );
    }

    WNDPROC RainGuiNvidia_DetachOrigWndProc()
    {
        return reinterpret_cast<WNDPROC>(
            InterlockedExchangePointer(&s_rainGuiNvidiaOrigWndProc, nullptr)
        );
    }

    HWND RainGuiNvidia_GetHookedWindow()
    {
        return reinterpret_cast<HWND>(
            InterlockedCompareExchangePointer(&s_rainGuiNvidiaHookedWindow, nullptr, nullptr)
        );
    }

    void RainGuiNvidia_SetHookedWindow(HWND hWnd)
    {
        InterlockedExchangePointer(&s_rainGuiNvidiaHookedWindow, hWnd);
    }

    void RainGuiNvidia_ClearHookedWindow()
    {
        InterlockedExchangePointer(&s_rainGuiNvidiaHookedWindow, nullptr);
    }

    void RainGuiNvidia_RestoreOriginalWndProc(HWND hWnd, WNDPROC originalWndProc)
    {
        if (!hWnd || !originalWndProc || !IsWindow(hWnd))
        {
            return;
        }

        LONG_PTR currentWndProc = GetWindowLongPtrA(hWnd, GWLP_WNDPROC);
        if (reinterpret_cast<WNDPROC>(currentWndProc) == RainGuiNvidiaWndProc)
        {
            SetWindowLongPtrA(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(originalWndProc));
        }
    }

    LRESULT CALLBACK RainGuiNvidiaWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        RainGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

        if (msg == WM_NCDESTROY)
        {
            WNDPROC originalWndProc = RainGuiNvidia_DetachOrigWndProc();
            RainGuiNvidia_ClearHookedWindow();
            if (originalWndProc)
            {
                RainGuiNvidia_RestoreOriginalWndProc(hWnd, originalWndProc);
                return CallWindowProcA(originalWndProc, hWnd, msg, wParam, lParam);
            }

            return DefWindowProcA(hWnd, msg, wParam, lParam);
        }

        WNDPROC originalWndProc = RainGuiNvidia_GetOrigWndProc();
        if (originalWndProc)
        {
            return CallWindowProcA(originalWndProc, hWnd, msg, wParam, lParam);
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

    if (RainGuiNvidia_GetOrigWndProc())
    {
        return RainGuiNvidia_GetHookedWindow() == hWnd;
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

    WNDPROC originalWndProc = reinterpret_cast<WNDPROC>(previousWndProc);
    if (!originalWndProc)
    {
        return false;
    }

    PVOID exchangeResult = InterlockedCompareExchangePointer(
        &s_rainGuiNvidiaOrigWndProc,
        reinterpret_cast<PVOID>(originalWndProc),
        nullptr
    );
    if (exchangeResult != nullptr)
    {
        // 为什么恢复：避免并发重复安装时把窗口消息链写乱。
        SetWindowLongPtrA(hWnd, GWLP_WNDPROC, previousWndProc);
        return false;
    }

    RainGuiNvidia_SetHookedWindow(hWnd);
    return true;
}

inline void RainGuiNvidia_RemoveWndProcHook(HWND hWnd)
{
    WNDPROC originalWndProc = RainGuiNvidia_DetachOrigWndProc();
    HWND hookedWindow = hWnd ? hWnd : RainGuiNvidia_GetHookedWindow();
    RainGuiNvidia_ClearHookedWindow();

    if (!originalWndProc)
    {
        return;
    }

    RainGuiNvidia_RestoreOriginalWndProc(hookedWindow, originalWndProc);
}
