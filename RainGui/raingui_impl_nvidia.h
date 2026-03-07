#pragma once

// RainGui NVIDIA Overlay 后端
// 封装 NVIDIA GeForce Overlay 窗口劫持、D3D11 渲染管线、输入处理、反截图等功能
// 内部调用 raingui_impl_win32 和 raingui_impl_dx11，对外提供简化 API
//
// 用法:
//   RainGui::CreateContext();
//   RainGui_Nvidia_Init();         // 查找窗口 + 配置 + D3D11 + 初始化后端
//   // 设置字体/主题...
//   while (running)
//   {
//       RainGui_Nvidia_NewFrame();  // 维护窗口 + 输入 + 后端 NewFrame
//       RainGui::NewFrame();
//       // 绘制 GUI...
//       RainGui::Render();
//       RainGui_Nvidia_Present();   // 渲染 + 呈现
//   }
//   RainGui_Nvidia_Shutdown();      // 清理 + 恢复窗口

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11RenderTargetView;
struct IDXGISwapChain;

// 初始化：查找 NVIDIA 覆盖层窗口，配置窗口属性，创建 D3D11，初始化 RainGui 后端
// 返回 false 表示初始化失败（窗口未找到或 D3D11 创建失败）
bool RainGui_Nvidia_Init();

// 关闭：清理 D3D11/RainGui 后端，恢复 NVIDIA 窗口原始状态
void RainGui_Nvidia_Shutdown();

// 每帧调用：维护窗口置顶/尺寸，动态切换点击穿透，处理输入
// 在 RainGui::NewFrame() 之前调用
void RainGui_Nvidia_NewFrame();

// 呈现：清屏 + 渲染 RainGui DrawData + SwapChain Present
// 在 RainGui::Render() 之后调用
void RainGui_Nvidia_Present();

// 获取 NVIDIA 覆盖层窗口句柄
void* RainGui_Nvidia_GetHwnd();

// 获取 D3D11 设备和上下文（供外部使用）
ID3D11Device*        RainGui_Nvidia_GetDevice();
ID3D11DeviceContext* RainGui_Nvidia_GetDeviceContext();

// 获取屏幕物理分辨率
float RainGui_Nvidia_GetScreenWidth();
float RainGui_Nvidia_GetScreenHeight();
