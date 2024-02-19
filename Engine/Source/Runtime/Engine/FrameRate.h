#pragma once

#include "../Core/Types.h"
#include "Defines.h"
#include <chrono>

class FrameRate
{
	using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
public:
	void Init();
	double Update();
	void Destroy();

	double GetDelta() { return _ElapsedTime.count(); }
	u64 GetFrameCount() { return _FrameCount; }

	double GetAverageFrameDelta();

private:
	TimePoint _Now;
	std::chrono::duration<double> _ElapsedTime;

	u64 _FrameCount;
	std::vector<double> _FrameWindows;
	constexpr static u64 _FrameWindowSize = 60;
};