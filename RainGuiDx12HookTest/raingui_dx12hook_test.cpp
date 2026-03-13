#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wchar.h>

#include "../RainGui/raingui_dx12hook_types.h"

namespace
{
    typedef void (*FillDefaultDescFn)(RainGuiDx12HookDesc* desc);
    typedef bool (*InitDx12HookFn)(const RainGuiDx12HookDesc* desc);
    typedef void (*ShutdownDx12HookFn)();

    HMODULE g_selfModule = nullptr;

    bool GetModuleDirectory(HMODULE moduleHandle, wchar_t* buffer, size_t bufferCount)
    {
        if (!buffer || bufferCount == 0)
        {
            return false;
        }

        DWORD length = GetModuleFileNameW(moduleHandle, buffer, static_cast<DWORD>(bufferCount));
        if (length == 0 || length >= bufferCount)
        {
            return false;
        }

        for (DWORD index = length; index > 0; --index)
        {
            if (buffer[index - 1] == L'\\')
            {
                buffer[index] = L'\0';
                return true;
            }
        }

        return false;
    }

    bool BuildRainGuiPath(wchar_t* buffer, size_t bufferCount)
    {
        wchar_t moduleDirectory[MAX_PATH] = {};
        if (!GetModuleDirectory(g_selfModule, moduleDirectory, _countof(moduleDirectory)))
        {
            return false;
        }

        if (wcscpy_s(buffer, bufferCount, moduleDirectory) != 0)
        {
            return false;
        }

        return wcscat_s(buffer, bufferCount, L"..\\..\\RainGui\\bin\\RainGui.dll") == 0;
    }

    bool ResolveExports(
        HMODULE rainGuiModule,
        FillDefaultDescFn& fillDefaultDesc,
        InitDx12HookFn& initDx12Hook,
        ShutdownDx12HookFn& shutdownDx12Hook)
    {
        fillDefaultDesc = reinterpret_cast<FillDefaultDescFn>(
            GetProcAddress(rainGuiModule, "RainGui_DX12Hook_FillDefaultDesc"));
        initDx12Hook = reinterpret_cast<InitDx12HookFn>(
            GetProcAddress(rainGuiModule, "RainGui_DX12Hook_Init"));
        shutdownDx12Hook = reinterpret_cast<ShutdownDx12HookFn>(
            GetProcAddress(rainGuiModule, "RainGui_DX12Hook_Shutdown"));

        return fillDefaultDesc && initDx12Hook && shutdownDx12Hook;
    }

    void WaitForForeground()
    {
        for (;;)
        {
            DWORD foregroundProcessId = 0;
            GetWindowThreadProcessId(GetForegroundWindow(), &foregroundProcessId);
            if (foregroundProcessId == GetCurrentProcessId())
            {
                break;
            }

            Sleep(50);
        }
    }

    DWORD WINAPI MainThread(LPVOID)
    {
        WaitForForeground();

        wchar_t rainGuiPath[MAX_PATH] = {};
        if (!BuildRainGuiPath(rainGuiPath, _countof(rainGuiPath)))
        {
            FreeLibraryAndExitThread(g_selfModule, 0);
        }

        HMODULE rainGuiModule = LoadLibraryW(rainGuiPath);
        if (!rainGuiModule)
        {
            FreeLibraryAndExitThread(g_selfModule, 0);
        }

        FillDefaultDescFn fillDefaultDesc = nullptr;
        InitDx12HookFn initDx12Hook = nullptr;
        ShutdownDx12HookFn shutdownDx12Hook = nullptr;
        if (!ResolveExports(rainGuiModule, fillDefaultDesc, initDx12Hook, shutdownDx12Hook))
        {
            FreeLibrary(rainGuiModule);
            FreeLibraryAndExitThread(g_selfModule, 0);
        }

        RainGuiDx12HookDesc desc = {};
        fillDefaultDesc(&desc);
        desc.enableDefaultDebugWindow = true;
        desc.startVisible = true;
        desc.toggleVirtualKey = VK_INSERT;

        bool installed = false;
        for (UINT attempt = 0; attempt < 200; ++attempt)
        {
            if (initDx12Hook(&desc))
            {
                installed = true;
                break;
            }

            Sleep(100);
        }

        if (!installed)
        {
            FreeLibrary(rainGuiModule);
            FreeLibraryAndExitThread(g_selfModule, 0);
        }

        while (!(GetAsyncKeyState(VK_END) & 1))
        {
            Sleep(100);
        }

        shutdownDx12Hook();
        Sleep(200);
        FreeLibrary(rainGuiModule);
        FreeLibraryAndExitThread(g_selfModule, 0);
    }
}

BOOL APIENTRY DllMain(HMODULE moduleHandle, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(moduleHandle);
        g_selfModule = moduleHandle;
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
    }

    return TRUE;
}
