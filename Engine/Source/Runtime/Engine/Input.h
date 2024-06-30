#pragma once

enum class KeyType : u32
{
     MouseScrollUp = 0,
     MouseScrollDown,
     MousePosition,

     LeftMouseButton,
     RightMouseButton,
     MiddleMouseButton,

     BackSpace,
     Tab,
     Enter,
     Pause,

     CapsLock,
     Escape,
     SpaceBar,
     PageUp,
     PageDown,
     End,
     Home,

     Left,
     Up,
     Right,
     Down,

     Insert,
     Delete,

     Zero,
     One,
     Two,
     Three,
     Four,
     Five,
     Six,
     Seven,
     Eight,
     Nine,

     A,
     B,
     C,
     D,
     E,
     F,
     G,
     H,
     I,
     J,
     K,
     L,
     M,
     N,
     O,
     P,
     Q,
     R,
     S,
     T,
     U,
     V,
     W,
     X,
     Y,
     Z,

     NumPad0,
     NumPad1,
     NumPad2,
     NumPad3,
     NumPad4,
     NumPad5,
     NumPad6,
     NumPad7,
     NumPad8,
     NumPad9,

     Multiply,
     Add,
     Subtract,
     Decimal,
     Divide,

     F1,
     F2,
     F3,
     F4,
     F5,
     F6,
     F7,
     F8,
     F9,
     F10,
     F11,
     F12,

     MaxNum,
};

enum class KeyModifier : u32
{
    None = 0x0,
    Shift = 0x1,
    Ctrl = 0x2,
    Alt = 0x4,
    Super = 0x8,
    CapsLock = 0x10,
    NumLock = 0x20,
};

struct KeyEvent
{
    enum class ActionType
    {
        Pressed,
        Released,
    };

    ActionType type;
    KeyType key;
    u32 modifier;
};

struct MouseEvent
{
    enum class ActionType
    {
        Pressed,
        Released,
        DbClick,   //double click
        Move,
    };

    ActionType type;
    KeyType key;
    u32 modifier;
    i32 xPos, yPos;
    f32 delta;
};
 

class InputHandler
{
public:
    virtual bool OnKey(const KeyEvent& key) = 0;
    virtual bool OnMouse(const MouseEvent& mev) = 0;
};

class InputManager
{
public:

    ENGINE_API static InputManager& Instance();

    ENGINE_API void OnKey(const KeyEvent& key);
    ENGINE_API void OnMouse(const MouseEvent& mev);

    ENGINE_API void AddHandler(InputHandler* handler);
    ENGINE_API void RemoveHandler(InputHandler* handler);

private:
    InputManager() = default;
    InputManager(const InputManager& rhs) = delete;
    InputManager(InputManager&& rhs) = delete;
    InputManager& operator=(const InputManager& rhs) = delete;
    InputManager& operator=(InputManager&& rhs) = delete;

private:
    std::vector<InputHandler*> _Handlers;
};