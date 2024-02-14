#pragma once

#include "Defines.h"

#include <concepts>

enum class RenderDeviceType
{
	DX12,
	Vulkan,
	Metal,
};

template <typename T>
concept IRenderDevice = requires(T t) {
	t.Init();
	t.Destroy();
};


template <IRenderDevice T>
class RenderDevice : public T
{
public:

};
