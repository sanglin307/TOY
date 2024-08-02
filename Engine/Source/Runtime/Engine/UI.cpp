#include "Private.h"

#include "imgui.h"

#ifdef WINDOWS
#include "backends/imgui_impl_win32.h"
#endif

UI& UI::Instance()
{
	static UI Inst;
	return Inst;
}

void UI::Init(void* hwnd)
{
	IMGUI_CHECKVERSION();
	_ImGuiContext = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

#ifdef WINDOWS
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
#endif

	GameEngine::Instance().GetRHI().ImGuiInit((void*)_ImGuiContext);
}

void UI::Destroy()
{
	GameEngine::Instance().GetRHI().ImGuiDestroy();
#ifdef WINDOWS
	ImGui_ImplWin32_Shutdown();
#endif
	ImGui::DestroyContext((ImGuiContext*)_ImGuiContext);
}

void UI::Update()
{
	GameEngine::Instance().GetRHI().ImGuiNewFrame();

#ifdef WINDOWS
	ImGui_ImplWin32_NewFrame();
#endif

	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
}