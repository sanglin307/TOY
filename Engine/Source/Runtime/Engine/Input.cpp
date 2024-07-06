#include "Private.h"

#if _DEBUG
#include <Windows.h>
#endif

InputManager& InputManager::Instance()
{
    static InputManager Inst;
    return Inst;
}

InputManager::InputManager()
{
    _PersistentKeyState.reset();
}

bool InputManager::IsKeyDown(KeyType key)
{
    return _PersistentKeyState.test((u32)key);
}

void InputManager::OnKey(const KeyEvent& key)
{
    _PersistentKeyState.set((u32)key.Key, key.Type == KeyEvent::ActionType::Pressed ? true : false);
    for (auto iter : _Handlers)
    {
        iter->OnKey(key);
    }
}

void InputManager::OnMouse(const MouseEvent& mev)
{
    if (mev.Type == MouseEvent::ActionType::Pressed)
    {
        _PersistentKeyState.set((u32)mev.Key, true);
    }
    else if (mev.Type == MouseEvent::ActionType::Released)
    {
        _PersistentKeyState.set((u32)mev.Key, false);
    }

    if (mev.Type == MouseEvent::ActionType::Move)
    {
        _MousePosition = int2(mev.XPos, mev.YPos);
    }
    
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