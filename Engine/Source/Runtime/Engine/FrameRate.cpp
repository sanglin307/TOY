#include "Private.h"

void FrameRate::Init()
{
	_FrameCount = 0;
	_FrameWindows.resize(_FrameWindowSize);
	_Now = std::chrono::steady_clock::now();
	_ElapsedTime = std::chrono::seconds(0);
}

void FrameRate::Destroy()
{
}

double FrameRate::Update()
{
	TimePoint now = std::chrono::steady_clock::now();
	_ElapsedTime = now - _Now;
	_FrameWindows[_FrameCount % _FrameWindowSize] = _ElapsedTime.count();
	_Now = now;
	_FrameCount++;
	return _ElapsedTime.count();
}

double FrameRate::GetAverageFrameDelta()
{
	u64 frames = std::min(_FrameCount, _FrameWindowSize);
	double time = 0;
	for (u64 i = 0; i < frames; i++)
		time += _FrameWindows[i];
	return time / double(frames);
}