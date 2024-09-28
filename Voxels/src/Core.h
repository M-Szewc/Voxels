#pragma once

#ifdef VO_PLATFORM_WINDOWS

#ifdef VO_DEBUG

//#define USE_VALIDATION_LAYER

#endif

#else
	#error OnlyWindowsSupported
#endif