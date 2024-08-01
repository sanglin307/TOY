#include "Private.h"

#include "imgui.h"
#include "backends/imgui_impl_win32.h"

#include "imgui_internal.h"

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

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);

	GameEngine::Instance().GetRHI().ImGuiInit((void*)_ImGuiContext);
}

void UI::Destroy()
{
	GameEngine::Instance().GetRHI().ImGuiDestroy();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext((ImGuiContext*)_ImGuiContext);
}

void UI::Update()
{
	GameEngine::Instance().GetRHI().ImGuiNewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
}