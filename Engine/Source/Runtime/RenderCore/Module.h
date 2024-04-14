#pragma once

class IRendererModule : public ModuleInterface
{
public:
    virtual void Init() override {}
    virtual void Destroy() override {}

    virtual void CreateRenderer(std::any hwnd, const RenderConfig& config) = 0;
    virtual void Render() = 0;

private:

};