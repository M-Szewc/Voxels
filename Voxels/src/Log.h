#pragma once

#include "Base.h"
#include "spdlog/spdlog.h"

namespace Game {

	class Log
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
#define VO_CORE_ERROR(...)	Game::Log::GetCoreLogger()->error(__VA_ARGS__)
#define VO_CORE_WARN(...)	Game::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define VO_CORE_INFO(...)	Game::Log::GetCoreLogger()->info(__VA_ARGS__)
#define VO_CORE_TRACE(...)	Game::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define VO_CORE_FATAL(...)	Game::Log::GetCoreLogger()->critical(__VA_ARGS__)

// vulkan log macros
#define VO_VULKAN_ERROR(...)	Game::Log::GetVulkanLogger()->error(__VA_ARGS__)
#define VO_VULKAN_WARN(...)		Game::Log::GetVulkanLogger()->warn(__VA_ARGS__)
#define VO_VULKAN_INFO(...)		Game::Log::GetVulkanLogger()->info(__VA_ARGS__)
#define VO_VULKAN_TRACE(...)	Game::Log::GetVulkanLogger()->trace(__VA_ARGS__)
#define VO_VULKAN_FATAL(...)	Game::Log::GetVulkanLogger()->critical(__VA_ARGS__)

// game log macros
#define VO_ERROR(...)	Game::Log::GetGameLogger()->error(__VA_ARGS__)
#define VO_WARN(...)	Game::Log::GetGameLogger()->warn(__VA_ARGS__)
#define VO_INFO(...)	Game::Log::GetGameLogger()->info(__VA_ARGS__)
#define VO_TRACE(...)	Game::Log::GetGameLogger()->trace(__VA_ARGS__)
#define VO_FATAL(...)	Game::Log::GetGameLogger()->critical(__VA_ARGS__)

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