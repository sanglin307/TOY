#pragma once


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN   
#endif

#define NOMINMAX

#include <windows.h>
#include <windowsx.h>
#include <WinUser.h>
#include <shellapi.h>

#define HLSLPP_FEATURE_TRANSFORM
#include "hlsl++.h"
using namespace hlslpp;

#include "../Core/Public.h"

#include "CommonStruct.h"

#include "../RHI/Public.h"
#include "../Engine/Public.h"
