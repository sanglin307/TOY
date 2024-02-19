#include "Input.h"

InputManager& InputManager::Instance()
{
    static InputManager Inst;
    return Inst;
}

void InputManager::OnKey(const KeyEvent& key)
{

}

void InputManager::OnMouse(const MouseEvent& mev)
{

}