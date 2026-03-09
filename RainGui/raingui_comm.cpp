// RainGui 共享内存通信实现

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include "raingui_comm.h"

// ============================================================
// 内部状态
// ============================================================

static HANDLE s_commMap = nullptr;
static RainGuiCommData* s_commData = nullptr;
static DWORD s_lastFrameId = 0;
static ULONGLONG s_lastHeartbeatTick = 0;

namespace
{
    constexpr ULONGLONG kCommOfflineTimeoutMs = 1500;

    void ResetOfflineState()
    {
        if (!s_commData)
        {
            return;
        }

        s_commData->commandCount = 0;
        s_commData->heartbeatTick = 0;
        s_commData->processName[0] = '\0';
    }
}

// ============================================================
// 公开 API
// ============================================================

bool RainGui_Comm_Init()
{
    fprintf(stderr, "[RainGuiComm] 初始化共享内存通信...\n");
    fflush(stderr);

    // 创建安全描述符（允许所有人访问）
    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = &sd;
    sa.bInheritHandle = FALSE;

    // 先尝试打开已存在的共享内存
    fprintf(stderr, "[RainGuiComm] 尝试打开共享内存: %s\n", RAINGUI_COMM_NAME);
    fflush(stderr);

    s_commMap = OpenFileMappingA(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        RAINGUI_COMM_NAME
    );

    bool needInit = false;

    if (s_commMap == nullptr)
    {
        DWORD err = GetLastError();
        fprintf(stderr, "[RainGuiComm] OpenFileMapping 失败: %lu, 尝试创建...\n", err);
        fflush(stderr);

        // 如果不存在，就创建它
        s_commMap = CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            &sa,
            PAGE_READWRITE,
            0,
            RAINGUI_COMM_SIZE,
            RAINGUI_COMM_NAME
        );

        if (s_commMap == nullptr)
        {
            fprintf(stderr, "[RainGuiComm] CreateFileMapping 失败: %lu\n", GetLastError());
            fflush(stderr);
            return false;
        }

        needInit = (GetLastError() != ERROR_ALREADY_EXISTS);
        fprintf(stderr, "[RainGuiComm] CreateFileMapping 成功, 句柄: %p, 需要初始化: %d\n",
            (void*)s_commMap, needInit);
        fflush(stderr);
    }
    else
    {
        fprintf(stderr, "[RainGuiComm] OpenFileMapping 成功, 句柄: %p\n", (void*)s_commMap);
        fflush(stderr);
    }

    // 映射到虚拟地址空间
    s_commData = (RainGuiCommData*)MapViewOfFile(
        s_commMap,
        FILE_MAP_ALL_ACCESS,     // 改为读写权限
        0, 0,
        RAINGUI_COMM_SIZE
    );

    if (s_commData == nullptr)
    {
        fprintf(stderr, "[RainGuiComm] MapViewOfFile 失败: %lu\n", GetLastError());
        fflush(stderr);
        CloseHandle(s_commMap);
        s_commMap = nullptr;
        return false;
    }

    fprintf(stderr, "[RainGuiComm] MapViewOfFile 成功, 虚拟地址: %p\n", (void*)s_commData);
    fflush(stderr);

    // 验证魔数和版本
    if (s_commData->magic != RAINGUI_COMM_MAGIC)
    {
        fprintf(stderr, "[RainGuiComm] 魔数不匹配: 0x%08X (期望 0x%08X), 可能需要初始化\n",
            s_commData->magic, RAINGUI_COMM_MAGIC);
        fflush(stderr);
        needInit = true;
    }
    if (s_commData->version != RAINGUI_COMM_VERSION)
    {
        fprintf(stderr, "[RainGuiComm] 版本不匹配: %u (期望 %u), 重新初始化共享内存\n",
            s_commData->version, RAINGUI_COMM_VERSION);
        fflush(stderr);
        needInit = true;
    }

    if (needInit)
    {
        fprintf(stderr, "[RainGuiComm] 初始化共享内存数据...\n");
        fflush(stderr);

        memset(s_commData, 0, RAINGUI_COMM_SIZE);
        s_commData->magic = RAINGUI_COMM_MAGIC;
        s_commData->version = RAINGUI_COMM_VERSION;
        s_commData->frameId = 0;
        s_commData->commandCount = 0;
        s_commData->heartbeatTick = 0;

        // 初始化默认配置
        s_commData->config.espEnabled = 1;
        s_commData->config.drawPlayerSkeleton = 1;
        s_commData->config.drawZombieSkeleton = 1;
        s_commData->config.drawNames = 0;
        s_commData->config.drawCollision = 0;
        s_commData->config.actorFilterEnabled = 1;
        s_commData->config.aimbotEnabled = 0;
        s_commData->config.drawFOV = 1;
        s_commData->config.fovRadius = 100.0f;
        s_commData->config.smoothing = 5.0f;
        s_commData->config.deadZone = 2.0f;
        s_commData->config.tickDelay = 0;
        s_commData->config.targetBone = 0;
        s_commData->config.aimKey = VK_RBUTTON;
        s_commData->config.showUI = 1;
        strcpy_s(s_commData->config.actorClassFilter, "Zombie");
    }

    s_lastFrameId = s_commData->frameId;
    s_lastHeartbeatTick = s_commData->heartbeatTick;
    fprintf(stderr, "[RainGuiComm] 初始化成功, frameId=%u, commandCount=%u\n",
        s_commData->frameId, s_commData->commandCount);
    fflush(stderr);

    return true;
}

void RainGui_Comm_Shutdown()
{
    if (s_commData)
    {
        UnmapViewOfFile(s_commData);
        s_commData = nullptr;
    }

    if (s_commMap)
    {
        CloseHandle(s_commMap);
        s_commMap = nullptr;
    }

    fprintf(stderr, "[RainGuiComm] 共享内存通信已关闭\n");
    fflush(stderr);
}

const RainGuiCommData* RainGui_Comm_GetData()
{
    return s_commData;
}

RainGuiConfig* RainGui_Comm_GetConfig()
{
    if (!s_commData)
        return nullptr;

    // 返回可写指针（用于 UI 修改配置）
    return &(const_cast<RainGuiCommData*>(s_commData)->config);
}

bool RainGui_Comm_IsValid()
{
    if (!s_commData)
        return false;

    // 检查魔数是否仍然有效
    if (s_commData->magic != RAINGUI_COMM_MAGIC)
        return false;

    if (s_commData->version != RAINGUI_COMM_VERSION)
        return false;

    ULONGLONG heartbeatTick = s_commData->heartbeatTick;
    ULONGLONG now = GetTickCount64();
    if (heartbeatTick == 0 || now - heartbeatTick > kCommOfflineTimeoutMs)
    {
        if (heartbeatTick != s_lastHeartbeatTick || s_commData->commandCount != 0 || s_commData->processName[0] != '\0')
        {
            fprintf(stderr, "[RainGuiComm] 检测到通信离线，清理残留绘制命令\n");
            fflush(stderr);
            ResetOfflineState();
        }
        s_lastHeartbeatTick = 0;
        return false;
    }

    s_lastHeartbeatTick = heartbeatTick;
    s_lastFrameId = s_commData->frameId;
    return true;
}
