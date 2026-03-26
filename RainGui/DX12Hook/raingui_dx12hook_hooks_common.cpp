#include "raingui_dx12hook_internal.h"

extern LRESULT RainGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace RainGuiDx12HookInternal
{
    bool IsInteractiveVisible()
    {
        bool visible = false;

        if (g_state.desc.enableDefaultDebugWindow && g_state.defaultDebugVisible)
        {
            visible = true;
        }

        if (!g_state.desc.isVisible)
        {
            return visible;
        }

        __try
        {
            return visible || g_state.desc.isVisible(g_state.desc.userData);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            g_state.deviceLost = true;
            return visible;
        }
    }

    LRESULT CALLBACK HookedWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (g_state.unloading || !g_state.backendReady || !g_state.originalWndProc)
        {
            if (g_state.originalWndProc)
            {
                return CallWindowProcW(g_state.originalWndProc, hwnd, msg, wParam, lParam);
            }

            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }

        __try
        {
            if (g_state.rainGuiLockReady && TryAcquireSRWLockExclusive(&g_state.rainGuiLock))
            {
                RainGui::SetCurrentContext(g_state.context);
                LRESULT result = RainGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
                ReleaseSRWLockExclusive(&g_state.rainGuiLock);
                if (result)
                {
                    return TRUE;
                }
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }

        if (g_state.desc.blockInputWhenVisible && IsInteractiveVisible())
        {
            bool wantMouse = false;
            bool wantKeyboard = false;

            __try
            {
                if (g_state.context)
                {
                    RainGui::SetCurrentContext(g_state.context);
                    RainGuiIO& io = RainGui::GetIO();
                    wantMouse = io.WantCaptureMouse;
                    wantKeyboard = io.WantCaptureKeyboard || io.WantTextInput;
                }
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
            }

            switch (msg)
            {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_LBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_RBUTTONDBLCLK:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_MBUTTONDBLCLK:
            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
            case WM_MOUSEMOVE:
                if (wantMouse)
                {
                    return TRUE;
                }
                break;
            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_CHAR:
                if (wantKeyboard)
                {
                    return TRUE;
                }
                break;
            default:
                break;
            }
        }

        return CallWindowProcW(g_state.originalWndProc, hwnd, msg, wParam, lParam);
    }

    PresentFn ResolvePresentFn(IDXGISwapChain* swapChain)
    {
        if (swapChain)
        {
            auto** vtable = *reinterpret_cast<void***>(swapChain);
            if (vtable)
            {
                auto fn = reinterpret_cast<PresentFn>(vtable[8]);
                if (fn && fn != HookPresent)
                {
                    return fn;
                }
            }
        }

        return g_state.originalPresent;
    }

    Present1Fn ResolvePresent1Fn(IDXGISwapChain1* swapChain)
    {
        if (swapChain)
        {
            auto** vtable = *reinterpret_cast<void***>(swapChain);
            if (vtable)
            {
                auto fn = reinterpret_cast<Present1Fn>(vtable[22]);
                if (fn && fn != HookPresent1)
                {
                    return fn;
                }
            }
        }

        return g_state.originalPresent1;
    }

    ExecuteCommandListsFn ResolveExecuteFn(ID3D12CommandQueue* queue)
    {
        if (queue)
        {
            auto** vtable = *reinterpret_cast<void***>(queue);
            if (vtable)
            {
                auto fn = reinterpret_cast<ExecuteCommandListsFn>(vtable[10]);
                if (fn && fn != HookExecuteCommandLists)
                {
                    return fn;
                }
            }
        }

        return g_state.originalExecuteCommandLists;
    }

    HRESULT CallOriginalPresentSafe(
        IDXGISwapChain* swapChain,
        UINT syncInterval,
        UINT flags,
        DWORD* exceptionCode)
    {
        if (exceptionCode)
        {
            *exceptionCode = 0;
        }

        PresentFn fn = ResolvePresentFn(swapChain);
        if (!fn)
        {
            return E_FAIL;
        }

        __try
        {
            return fn(swapChain, syncInterval, flags);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            if (exceptionCode)
            {
                *exceptionCode = GetExceptionCode();
            }

            return E_FAIL;
        }
    }

    HRESULT CallOriginalPresent1Safe(
        IDXGISwapChain1* swapChain,
        UINT syncInterval,
        UINT flags,
        const DXGI_PRESENT_PARAMETERS* presentParameters,
        DWORD* exceptionCode)
    {
        if (exceptionCode)
        {
            *exceptionCode = 0;
        }

        Present1Fn fn = ResolvePresent1Fn(swapChain);
        if (!fn)
        {
            return E_FAIL;
        }

        __try
        {
            return fn(swapChain, syncInterval, flags, presentParameters);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            if (exceptionCode)
            {
                *exceptionCode = GetExceptionCode();
            }

            return E_FAIL;
        }
    }

    bool CallOriginalExecuteSafe(
        ID3D12CommandQueue* queue,
        UINT numCommandLists,
        ID3D12CommandList* const* commandLists)
    {
        ExecuteCommandListsFn fn = ResolveExecuteFn(queue);
        if (!fn)
        {
            return false;
        }

        fn(queue, numCommandLists, commandLists);
        return true;
    }

    void TryHookResizeBuffers1(IDXGISwapChain* swapChain)
    {
        if (g_state.originalResizeBuffers1 || !swapChain)
        {
            return;
        }

        auto** vtable = *reinterpret_cast<void***>(swapChain);
        if (!vtable)
        {
            return;
        }

        g_state.trackedSwapChainVtable = vtable;
        PatchVtable(
            vtable,
            39,
            reinterpret_cast<void*>(&HookResizeBuffers1),
            reinterpret_cast<void**>(&g_state.originalResizeBuffers1));
    }
}
