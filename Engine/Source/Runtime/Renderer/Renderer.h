#pragma once

#include "Device.h"
#include "../Engine/Engine.h"
#include "RenderPath.h"
#include "RenderScene.h"

enum class RenderAPI
{
    DX12,
    Vulkan,
    Metal,
};

enum class RenderPathType
{
    VisibilityBuffer = 0,
};

struct RenderConfig
{
    RenderAPI API;
    RenderPathType RenderPath;
    u32 FrameCount;
    u32 FrameWidth;
    u32 FrameHeight;

};

class Renderer
{
public:
    RENDERER_API static Renderer& Instance();
    RENDERER_API void ParseCmds(const std::set<std::string>& cmds);
    RENDERER_API void Init(std::any hwnd);
    RENDERER_API void Destroy();
    RENDERER_API void Render();
    RENDERER_API const RenderConfig& Config() const { return _Config; }
    const std::any HWND() const { return _HWND; }

private:
    Renderer() = default;
    Renderer(const Renderer& rhs) = delete;
    Renderer(Renderer&& rhs) = delete;
    Renderer& operator=(const Renderer& rhs) = delete;
    Renderer& operator=(Renderer&& rhs) = delete;

private:

    RenderConfig _Config;
    RenderScene* _Scene;
    RenderPath* _RenderPath;
    std::any _HWND;

};