#pragma once

#ifdef VO_PLATFORM_WINDOWS

	#ifdef VO_BUILD_DLL
		#define VO_API __declspec(dllexport)
	#else
		#define VO_API __declspec(dllimport)
	#endif	

#else
	#error "Only windows is supported!"
#endif

#ifdef VO_DEBUG

	#ifdef VO_PLATFORM_WINDOWS	
		#define VO_DEBUGBREAK() __debugbreak()
	#else
		#error "Platform doesn't support debugbreak yet"
	#endif

	// Validation Layer may cause memory leaks when used with push constants for some reason
	#define USE_VALIDATION_LAYER
	#define VO_ENABLE_ASSERTS
	#define VO_TRACE_EVENTS
#else
	#define VO_DEBUGBREAK()
#endif

#define VO_EXPAND_MACRO(x) x
#define VO_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define VO_LOG_FORMATTER(class) template <> struct fmt::formatter<class> : fmt::ostream_formatter {};