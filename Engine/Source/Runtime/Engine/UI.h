#pragma once

class UI
{
public:
	ENGINE_API static UI& Instance();

	ENGINE_API void Init(void* hwnd);
	ENGINE_API void Destroy();

	ENGINE_API void Update();

	ENGINE_API void* GetContext() {
		return _ImGuiContext;
	}

private:
	UI() = default;
	UI(const UI& rhs) = delete;
	UI(UI&& rhs) = delete;
	UI& operator=(const UI& rhs) = delete;
	UI& operator=(UI&& rhs) = delete;
 
private:
	void* _ImGuiContext = nullptr;
};