#pragma once
#include "Base.h"

namespace vkInit {

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);


	vk::DebugUtilsMessengerEXT CreateDebugMessanger(vk::Instance& instance, vk::DispatchLoaderDynamic& dldi);

	// logs device properties
	void LogDeviceProperties(const vk::PhysicalDevice& device);

	std::vector<std::string> TransformBitsToString(vk::SurfaceTransformFlagsKHR bits);

	std::vector<std::string> AlphaCompositeBitsToString(vk::CompositeAlphaFlagsKHR bits);

	std::vector<std::string> ImageUsageBitsToString(vk::ImageUsageFlags bits);

	std::string PresentModeInfo(vk::PresentModeKHR presentMode);

}