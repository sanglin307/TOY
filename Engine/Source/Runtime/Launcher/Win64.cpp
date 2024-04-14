#include "Win64.h"

// for DirectX12 Agility SDK.
extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 611; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }

static i32 sKeyCodeMap[0xff];
struct WindowsKeyCodeMapInitializer
{
    WindowsKeyCodeMapInitializer()
    {
        std::memset(sKeyCodeMap, -1, sizeof(u32) * 0xff);
#define KEYCODE(VK,KEY)  check(VK < 0xff); sKeyCodeMap[VK] = static_cast<i32>(KEY);
        KEYCODE(VK_LBUTTON, KeyType::LeftMouseButton);
        KEYCODE(VK_RBUTTON, KeyType::RightMouseButton);
        KEYCODE(VK_MBUTTON, KeyType::MiddleMouseButton);
        KEYCODE(VK_BACK, KeyType::BackSpace);
        KEYCODE(VK_TAB, KeyType::Tab);
        KEYCODE(VK_PAUSE, KeyType::Pause);
        KEYCODE(VK_CAPITAL, KeyType::CapsLock);
        KEYCODE(VK_ESCAPE, KeyType::Escape);
        KEYCODE(VK_SPACE, KeyType::SpaceBar);
        KEYCODE(VK_PRIOR, KeyType::PageUp);
        KEYCODE(VK_NEXT, KeyType::PageDown);
        KEYCODE(VK_END, KeyType::End);
        KEYCODE(VK_HOME, KeyType::Home);
        KEYCODE(VK_LEFT, KeyType::Left);
        KEYCODE(VK_UP, KeyType::Up);
        KEYCODE(VK_DOWN, KeyType::Down);
        KEYCODE(VK_INSERT, KeyType::Insert);
        KEYCODE(VK_DELETE, KeyType::Delete);
        KEYCODE(0x30, KeyType::Zero);
        KEYCODE(0x31, KeyType::One);
        KEYCODE(0x32, KeyType::Two);
        KEYCODE(0x33, KeyType::Three);
        KEYCODE(0x34, KeyType::Four);
        KEYCODE(0x35, KeyType::Five);
        KEYCODE(0x36, KeyType::Six);
        KEYCODE(0x37, KeyType::Seven);
        KEYCODE(0x38, KeyType::Eight);
        KEYCODE(0x39, KeyType::Nine);
        KEYCODE(0x41, KeyType::A);
        KEYCODE(0x42, KeyType::B);
        KEYCODE(0x43, KeyType::C);
        KEYCODE(0x44, KeyType::D);
        KEYCODE(0x45, KeyType::E);
        KEYCODE(0x46, KeyType::F);
        KEYCODE(0x47, KeyType::G);
        KEYCODE(0x48, KeyType::H);
        KEYCODE(0x49, KeyType::I);
        KEYCODE(0x4A, KeyType::J);
        KEYCODE(0x4B, KeyType::K);
        KEYCODE(0x4C, KeyType::L);
        KEYCODE(0x4D, KeyType::M);
        KEYCODE(0x4E, KeyType::N);
        KEYCODE(0x4F, KeyType::O);
        KEYCODE(0x50, KeyType::P);
        KEYCODE(0x51, KeyType::Q);
        KEYCODE(0x52, KeyType::R);
        KEYCODE(0x53, KeyType::S);
        KEYCODE(0x54, KeyType::T);
        KEYCODE(0x55, KeyType::U);
        KEYCODE(0x56, KeyType::V);
        KEYCODE(0x57, KeyType::W);
        KEYCODE(0x58, KeyType::X);
        KEYCODE(0x59, KeyType::Y);
        KEYCODE(0x5A, KeyType::Z);

        KEYCODE(VK_NUMPAD0, KeyType::NumPad0);
        KEYCODE(VK_NUMPAD1, KeyType::NumPad1);
        KEYCODE(VK_NUMPAD2, KeyType::NumPad2);
        KEYCODE(VK_NUMPAD3, KeyType::NumPad3);
        KEYCODE(VK_NUMPAD4, KeyType::NumPad4);
        KEYCODE(VK_NUMPAD5, KeyType::NumPad5);
        KEYCODE(VK_NUMPAD6, KeyType::NumPad6);
        KEYCODE(VK_NUMPAD7, KeyType::NumPad7);
        KEYCODE(VK_NUMPAD8, KeyType::NumPad8);
        KEYCODE(VK_NUMPAD9, KeyType::NumPad9);

        KEYCODE(VK_MULTIPLY, KeyType::Multiply);
        KEYCODE(VK_ADD, KeyType::Add);
        KEYCODE(VK_SUBTRACT, KeyType::Subtract);
        KEYCODE(VK_DECIMAL, KeyType::Decimal);
        KEYCODE(VK_DIVIDE, KeyType::Divide);

        KEYCODE(VK_F1, KeyType::F1);
        KEYCODE(VK_F2, KeyType::F2);
        KEYCODE(VK_F3, KeyType::F3);
        KEYCODE(VK_F4, KeyType::F4);
        KEYCODE(VK_F5, KeyType::F5);
        KEYCODE(VK_F6, KeyType::F6);
        KEYCODE(VK_F7, KeyType::F7);
        KEYCODE(VK_F8, KeyType::F8);
        KEYCODE(VK_F9, KeyType::F9);
        KEYCODE(VK_F10, KeyType::F10);
        KEYCODE(VK_F11, KeyType::F11);
        KEYCODE(VK_F12, KeyType::F12);

#undef KEYCODE
    }
};
WindowsKeyCodeMapInitializer sKeyCodeMapInitializer;
 

static u32 KeyMods(void)
{
    u32 mods = 0;

    if (GetKeyState(VK_SHIFT) & 0x8000)
        mods |= static_cast<u32>(KeyModifier::Shift);
    if (GetKeyState(VK_CONTROL) & 0x8000)
        mods |= static_cast<u32>(KeyModifier::Ctrl);
    if (GetKeyState(VK_MENU) & 0x8000)
        mods |= static_cast<u32>(KeyModifier::Alt);
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
        mods |= static_cast<u32>(KeyModifier::Super);
    if (GetKeyState(VK_CAPITAL) & 1)
        mods |= static_cast<u32>(KeyModifier::CapsLock);
    if (GetKeyState(VK_NUMLOCK) & 1)
        mods |= static_cast<u32>(KeyModifier::NumLock);

    return mods;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        {    
            const u32 key = static_cast<u32>(wParam);
            check(key < 0xff);
            i32 keyCode = sKeyCodeMap[key];
            if (keyCode >= 0)
            {
                KeyEvent keyEvent = { 
                    .type = (HIWORD(lParam) & KF_UP) ? KeyEvent::ActionType::Released : KeyEvent::ActionType::Pressed,
                    .key = static_cast<KeyType>(keyCode),
                    .modifier = KeyMods(),                  
                };
                InputManager::Instance().OnKey(keyEvent);
            }
        }
        return 0;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
        {
            auto action = MouseEvent::ActionType::Pressed;
            if (message == WM_LBUTTONUP || message == WM_RBUTTONUP || message == WM_MBUTTONUP)
                action = MouseEvent::ActionType::Released;
            else if (message == WM_LBUTTONDBLCLK || message == WM_RBUTTONDBLCLK || message == WM_MBUTTONDBLCLK)
                action = MouseEvent::ActionType::DbClick;

            auto keyType = KeyType::LeftMouseButton;
            if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
                keyType = KeyType::RightMouseButton;
            else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
                keyType = KeyType::MiddleMouseButton;

   
            MouseEvent mouseEvent = {
                        .type = action,
                        .key = keyType,
                        .modifier = KeyMods(),
                        .xPos = GET_X_LPARAM(lParam),
                        .yPos = GET_Y_LPARAM(lParam),
            };
            InputManager::Instance().OnMouse(mouseEvent);
        }
        return 0;
    case WM_MOUSEMOVE:
        {
            MouseEvent mouseEvent = {
                            .type = MouseEvent::ActionType::Move,
                            .key = KeyType::MousePosition,
                            .modifier = KeyMods(),
                            .xPos = GET_X_LPARAM(lParam),
                            .yPos = GET_Y_LPARAM(lParam),
            };
            InputManager::Instance().OnMouse(mouseEvent);
        }
        return 0;
    case WM_MOUSEWHEEL:
        {
            const f32 delta = GET_WHEEL_DELTA_WPARAM(wParam) / (f32)WHEEL_DELTA;
            auto action = KeyType::MouseScrollUp;
            if (delta < 0)
                action = KeyType::MouseScrollDown;

            MouseEvent mouseEvent = {
                            .type = MouseEvent::ActionType::Pressed,
                            .key = KeyType::MousePosition,
                            .modifier = KeyMods(),
                            .xPos = GET_X_LPARAM(lParam),
                            .yPos = GET_Y_LPARAM(lParam),
                            .delta = delta,
            };
            InputManager::Instance().OnMouse(mouseEvent);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
 
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    std::set<std::string> cmdlineSet;
    for (i32 i = 1; i < argc; i++)
    {
        cmdlineSet.insert(PlatformUtils::UTF16ToUTF8(argv[i]));
    }
    LocalFree(argv);

    GameEngine::Instance().ParseCmds(cmdlineSet);

    // Initialize the window class.
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"TOY";
    RegisterClassEx(&windowClass);

    u32 Width, Height;
    GameEngine::Instance().FrameSize(Width, Height);
    RECT windowRect = { 0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height)};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindow(
        windowClass.lpszClassName,
        L"TOY",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,        
        nullptr,       
        hInstance,
        nullptr);
 
    std::any awnd = hwnd;
    GameEngine::Instance().Init(awnd);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        // Process any messages in the queue.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        GameEngine::Instance().Update();
    }

    GameEngine::Instance().Destroy();
    return static_cast<char>(msg.wParam);
}