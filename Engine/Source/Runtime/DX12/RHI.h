#pragma once

class DX12RHIModule final : public IRHIModule
{
public:
    virtual void Init() override;
    virtual void Destroy() override;
    virtual RenderDevice* GetDevice() override;
    virtual void ImGuiInit(void* ctx) override;
    virtual void ImGuiDestroy() override;
    virtual void ImGuiNewFrame() override;
};
 