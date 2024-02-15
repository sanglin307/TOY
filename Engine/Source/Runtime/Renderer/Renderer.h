#pragma once

#include "Device.h"
#include "Command.h"
#include "../Engine/Engine.h"


class RENDERER_API Renderer
{
public:
    static Renderer& Instance()
    {
        static Renderer Inst;
        return Inst;
    }

    void Init(const GameEngine::Config& config, void* hwnd);
    void Destroy();

private:
    Renderer() = default;
    Renderer(const Renderer& rhs) = delete;
    Renderer(Renderer&& rhs) = delete;
    Renderer& operator=(const Renderer& rhs) = delete;
    Renderer& operator=(Renderer&& rhs) = delete;

private:

    RenderDevice* _Device = nullptr;
    CommandQueue* _DirectQueue = nullptr;
    SwapChain* _SwapChain = nullptr;
    u32 _FrameIndex = 0;



};