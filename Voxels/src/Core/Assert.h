#pragma once

#include "Core/Log.h"

#ifdef VO_ENABLE_ASSERTS
	#define VO_ASSERT(x, ...) {if(!(x)) {VO_ERROR("Assertion Failed: {0}", __VA_ARGS__); VO_DEBUGBREAK();} }
	#define VO_CORE_ASSERT(x, ...) {if(!(x)) { VO_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); VO_DEBUGBREAK();} }
#else
	#define VO_ASSERT(x, ...)
	#define VO_CORE_ASSERT(x, ...)
#endif

#ifndef VO_BUILD_DLL
	#undef VO_CORE_ASSERT(x, ...)
#endif