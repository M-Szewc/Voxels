#pragma once

#include "Core/Base.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace VoxelEngine {

	class VO_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetVulkanLogger() { return s_VulkanLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetGameLogger() { return s_GameLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_VulkanLogger;
		static std::shared_ptr<spdlog::logger> s_GameLogger;
	};

}

// engine log macros
#define VO_CORE_ERROR(...)	VoxelEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VO_CORE_WARN(...)	VoxelEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VO_CORE_INFO(...)	VoxelEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VO_CORE_TRACE(...)	VoxelEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VO_CORE_FATAL(...)	VoxelEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// vulkan log macros
#define VO_VULKAN_ERROR(...)	VoxelEngine::Log::GetVulkanLogger()->error(__VA_ARGS__)
#define VO_VULKAN_WARN(...)		VoxelEngine::Log::GetVulkanLogger()->warn(__VA_ARGS__)
#define VO_VULKAN_INFO(...)		VoxelEngine::Log::GetVulkanLogger()->info(__VA_ARGS__)
#define VO_VULKAN_TRACE(...)	VoxelEngine::Log::GetVulkanLogger()->trace(__VA_ARGS__)
#define VO_VULKAN_FATAL(...)	VoxelEngine::Log::GetVulkanLogger()->critical(__VA_ARGS__)

// game log macros
#define VO_ERROR(...)	VoxelEngine::Log::GetGameLogger()->error(__VA_ARGS__)
#define VO_WARN(...)	VoxelEngine::Log::GetGameLogger()->warn(__VA_ARGS__)
#define VO_INFO(...)	VoxelEngine::Log::GetGameLogger()->info(__VA_ARGS__)
#define VO_TRACE(...)	VoxelEngine::Log::GetGameLogger()->trace(__VA_ARGS__)
#define VO_FATAL(...)	VoxelEngine::Log::GetGameLogger()->critical(__VA_ARGS__)

// clear engine and vulkan logs in distriution
#ifdef VO_DISTRIBUTION
#define VO_CORE_ERROR
#define VO_CORE_WARN
#define VO_CORE_INFO
#define VO_CORE_TRACE
#define VO_CORE_FATAL

#define VO_VULKAN_ERROR
#define VO_VULKAN_WARN
#define VO_VULKAN_INFO
#define VO_VULKAN_TRACE
#define VO_VULKAN_FATAL
#endif

#ifndef VO_BUILD_DLL
#undef VO_CORE_ERROR
#undef VO_CORE_WARN
#undef VO_CORE_INFO
#undef VO_CORE_TRACE
#undef VO_CORE_FATAL

#undef VO_VULKAN_ERROR
#undef VO_VULKAN_WARN
#undef VO_VULKAN_INFO
#undef VO_VULKAN_TRACE
#undef VO_VULKAN_FATAL
#endif