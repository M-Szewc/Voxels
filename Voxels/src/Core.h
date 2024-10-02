#pragma once

#ifdef VO_PLATFORM_WINDOWS

	#ifdef VO_BUILD_DLL
		#define VO_API __declspec(dllexport)
	#else
		#define VO_API __declspec(dllimport)
	#endif	

	#ifdef VO_DEBUG

		// Validation Layer may cause memory leaks when used with push constants for some reason
		#define USE_VALIDATION_LAYER

	#endif

#else
	#error Only windows is supported!
#endif