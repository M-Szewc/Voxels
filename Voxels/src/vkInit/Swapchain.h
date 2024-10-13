#pragma once
#include "Core/Base.h"

#include "vkInit/VulkanLogger.h"
#include "vkUtil/VulkanUtil.h"

namespace vkInit {

	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR Capabilities;
		std::vector<vk::SurfaceFormatKHR> Formats;
		std::vector<vk::PresentModeKHR> PresentModes;
	};

	struct SwapChainBundle {
		vk::SwapchainKHR Swapchain;
		std::vector<vkUtil::SwapChainFrame> Frames;
		vk::Format Format;
		vk::Extent2D Extent;
	};


	SwapChainSupportDetails QuerySwapchainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

	vk::SurfaceFormatKHR ChooseSwapchainSurfaceFormat(std::vector < vk::SurfaceFormatKHR> formats);

	vk::PresentModeKHR ChooseSwapchainPresentMode(std::vector<vk::PresentModeKHR> presentModes);

	vk::Extent2D ChooseSwapchainExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities);

	SwapChainBundle CreateSwapchain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height);
}