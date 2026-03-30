#pragma once

// RainGui 共享内存通信协议
// 用于外部进程向 NVIDIA 后端发送绘制命令

#include <windows.h>

// 绘制命令类型
enum RainGuiDrawCommandType
{
    RGUI_CMD_NONE = 0,
    RGUI_CMD_POINT = 1,        // 点（圆点）
    RGUI_CMD_LINE = 2,         // 线段
    RGUI_CMD_CIRCLE = 3,       // 圆圈
    RGUI_CMD_RECT = 4,         // 矩形
    RGUI_CMD_TEXT = 5,         // 文字
};

// 单个绘制命令
struct RainGuiDrawCommand
{
    DWORD type;                // 命令类型
    union
    {
        // 点（圆点）
        struct
        {
            float x, y;
            float radius;
            DWORD color;       // RGBA (ImGui 格式)
            BYTE visible;      // 可见性（0=不可见, 1=可见）
        } point;

        // 线段
        struct
        {
            float x1, y1, x2, y2;
            DWORD color;
            float thickness;
            BYTE visible;
        } line;

        // 圆圈
        struct
        {
            float x, y;
            float radius;
            DWORD color;
            float thickness;   // 0 = 填充
            BYTE visible;
        } circle;

        // 矩形
        struct
        {
            float x, y, w, h;
            DWORD color;
            float rounding;    // 圆角半径
            BYTE filled;       // 0=边框, 1=填充
            BYTE visible;
        } rect;

        // 文字
        struct
        {
            float x, y;
            DWORD color;
            float fontSize;
            char text[128];
        } text;
    };
};

// 配置变量结构（双向读写）
struct RainGuiConfig
{
    // ESP 配置
    BYTE espEnabled;                // ESP 总开关
    BYTE drawPlayerSkeleton;        // 绘制玩家骨骼
    BYTE drawZombieSkeleton;        // 绘制僵尸骨骼
    BYTE drawNames;                 // 绘制类名文本
    BYTE drawCollision;             // 绘制碰撞体
    BYTE actorFilterEnabled;        // 是否启用 Actor 类名过滤

    // Actor Overlay 配置
    BYTE drawActorText;             // 绘制 Actor 文字
    BYTE drawActorBoxes;            // 绘制 Actor 方框
    BYTE drawActorPoints;           // 绘制 Actor 点位
    BYTE excludeViewTarget;         // 排除当前视角 Actor
    float maxDistanceMeters;        // 最大绘制距离（0=不限）
    DWORD maxActors;                // 最大绘制 Actor 数量
    DWORD textColor;                // 文本颜色（RainGui ImU32）
    DWORD boxColor;                 // 方框颜色（RainGui ImU32）
    DWORD pointColor;               // 点位颜色（RainGui ImU32）

    // Aimbot 配置
    BYTE aimbotEnabled;             // Aimbot 总开关
    BYTE drawFOV;                   // 显示 FOV 圆圈
    float fovRadius;                // FOV 半径 (20-500)
    float smoothing;                // 平滑度 (1-30)
    float deadZone;                 // 死区 (0-10)
    DWORD tickDelay;                // 自瞄频率 (0-15ms)
    DWORD targetBone;               // 目标骨骼 (0=Head, 1=Neck)
    DWORD aimKey;                   // 自瞄按键 (VK_*)

    // UI 配置
    BYTE showUI;                    // 是否显示 UI
    BYTE reserved[3];               // 对齐
    char actorClassFilter[64];      // Actor 类名过滤关键字
};

// 共享内存数据结构
struct RainGuiCommData
{
    DWORD magic;               // 0x52474349 ('RGCI')
    DWORD version;             // 协议版本 (3)
    DWORD frameId;             // 帧 ID（用于检测更新）
    ULONGLONG heartbeatTick;   // 发送端心跳，用于离线检测
    char processName[64];      // 通信进程名

    // 配置变量（双向读写）
    RainGuiConfig config;

    // 绘制命令缓冲区
    DWORD commandCount;        // 当前命令数量
    RainGuiDrawCommand commands[32768];  // 最多 32768 条命令
};

// 共享内存大小估算：
// 头部 + 32768 条命令，约 4.4MB，用于避免复杂碰撞场景提前截断

#define RAINGUI_MAX_COMMANDS 32768
#define RAINGUI_COMM_MAGIC   0x52474349
#define RAINGUI_COMM_VERSION 4
#define RAINGUI_COMM_NAME    "Global\\RainGuiComm"
#define RAINGUI_COMM_SIZE    (sizeof(RainGuiCommData))

// 初始化共享内存通信（在 RainGui_Nvidia_Init 内部自动调用）
bool RainGui_Comm_Init();

// 关闭共享内存通信
void RainGui_Comm_Shutdown();

// 获取共享内存数据指针（只读，用于绘制）
const RainGuiCommData* RainGui_Comm_GetData();

// 获取共享内存数据指针（读写，用于采集端写入命令）
RainGuiCommData* RainGui_Comm_GetWritableData();

// 获取配置指针（可读写，用于 UI）
RainGuiConfig* RainGui_Comm_GetConfig();

// 检查共享内存是否有效
bool RainGui_Comm_IsValid();
