#include "Private.h"

InputManager& InputManager::Instance()
{
    static InputManager Inst;
    return Inst;
}

void InputManager::OnKey(const KeyEvent& key)
{
    for (auto iter : _Handlers)
    {
        iter->OnKey(key);
    }
}

void InputManager::OnMouse(const MouseEvent& mev)
{
    for (auto iter : _Handlers)
    {
        iter->OnMouse(mev);
    }
}

void InputManager::AddHandler(InputHandler* handler)
{
    auto iter = std::find(_Handlers.begin(), _Handlers.end(), handler);
    if (iter != _Handlers.end())
        return;

    _Handlers.push_back(handler);
}

void InputManager::RemoveHandler(InputHandler* handler)
{
    auto iter = std::find(_Handlers.begin(), _Handlers.end(), handler);
    if (iter == _Handlers.end())
        return;

    _Handlers.erase(iter);
}