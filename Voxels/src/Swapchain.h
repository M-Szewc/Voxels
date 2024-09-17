#pragma once
#include "vopch.h"

#include "VulkanLogger.h"
#include "VulkanUtil.h"

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


	SwapChainSupportDetails QuerySwapchainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		SwapChainSupportDetails support;

		support.Capabilities = device.getSurfaceCapabilitiesKHR(surface);

#ifdef VO_DEBUG
		VO_CORE_INFO("Swapchain can support the following surface capabilities:\n\tminimum image count: {0}\n\tmaximum image count: {1}",
			support.Capabilities.minImageCount, support.Capabilities.maxImageCount);

		VO_CORE_INFO("Current extent:\n\t\twidth: {0}\n\t\theight: {1}\n\tminimum supported extent:\n\t\twidth: {2}\n\t\theight: {3}\n\tmaximum supported extent:\n\t\twidth: {4}\n\t\theight: {5}",
			support.Capabilities.currentExtent.width, support.Capabilities.currentExtent.height,
			support.Capabilities.minImageExtent.width, support.Capabilities.minImageExtent.height,
			support.Capabilities.maxImageExtent.width, support.Capabilities.maxImageExtent.height);

		VO_CORE_INFO("Maximum image array layers: {0}", support.Capabilities.maxImageArrayLayers);

		std::vector<std::string> stringList = TransformBitsToString(support.Capabilities.supportedTransforms);
		std::stringstream ss;

		ss << "Supported transforms:" << std::endl;
		for (std::string line : stringList) {
			ss << "\t\t" << line << std::endl;
		}

		ss << "\tcurrent transform" << std::endl;
		stringList = TransformBitsToString(support.Capabilities.currentTransform);
		for (std::string line : stringList) {
			ss << "\t\t" << line << std::endl;
		}

		VO_CORE_INFO(ss.str());

		ss.str("");
		ss << "Supported alpha operations:" << std::endl;
		stringList = AlphaCompositeBitsToString(support.Capabilities.supportedCompositeAlpha);
		for (std::string line : stringList) {
			ss << "\t\t" << line << std::endl;
		}
		VO_CORE_INFO(ss.str());

		ss.str("");
		ss << "Supported image usage:" << std::endl;
		stringList = ImageUsageBitsToString(support.Capabilities.supportedUsageFlags);
		for (std::string line : stringList) {
			ss << "\t\t" << line << std::endl;
		}
		VO_CORE_INFO(ss.str());

#endif

		support.Formats = device.getSurfaceFormatsKHR(surface);

#ifdef VO_DEBUG
		for (vk::SurfaceFormatKHR supportedFormat : support.Formats) {
			VO_CORE_INFO("Supported pixel format: {0}\nSupported color space: {1}",
				vk::to_string(supportedFormat.format), vk::to_string(supportedFormat.colorSpace));
		}
#endif

		support.PresentModes = device.getSurfacePresentModesKHR(surface);

#ifdef VO_DEBUG
		ss.str("");
		for (vk::PresentModeKHR presentMode : support.PresentModes) {
			ss << "\t" << PresentModeInfo(presentMode) << std::endl;
		}
		VO_CORE_INFO(ss.str());
#endif

		return support;
	}

	vk::SurfaceFormatKHR ChooseSwapchainSurfaceFormat(std::vector < vk::SurfaceFormatKHR> formats) {

		for (vk::SurfaceFormatKHR format : formats) {
			if (format.format == vk::Format::eB8G8R8A8Unorm &&
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return format;
			}
		}

		return formats[0];
	}

	vk::PresentModeKHR ChooseSwapchainPresentMode(std::vector<vk::PresentModeKHR> presentModes) {

		for (vk::PresentModeKHR presentMode : presentModes) {
			if (presentMode == vk::PresentModeKHR::eMailbox) {
				return presentMode;
			}
		}

		// it is guaranteed to be supported
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D ChooseSwapchainExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities) {
		// if it doesn't need to be specified
		if (capabilities.currentExtent != UINT32_MAX) {
			return capabilities.currentExtent;
		}

		// create extent width x height that fits within capabilities

		vk::Extent2D extent = { width, height };

		extent.width = std::min(
			capabilities.maxImageExtent.width,
			std::max(capabilities.minImageExtent.width, width)
		);

		extent.height = std::min(
			capabilities.maxImageExtent.height,
			std::max(capabilities.minImageExtent.height, height)
		);

		return extent;
	}

	SwapChainBundle CreateSwapchain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height) {

		SwapChainSupportDetails support = QuerySwapchainSupport(physicalDevice, surface);

		vk::SurfaceFormatKHR format = ChooseSwapchainSurfaceFormat(support.Formats);

		vk::PresentModeKHR presentMode = ChooseSwapchainPresentMode(support.PresentModes);

		vk::Extent2D extent = ChooseSwapchainExtent(width, height, support.Capabilities);

		uint32_t imageCount = std::min(
			support.Capabilities.maxImageCount,
			support.Capabilities.minImageCount + 1
		);

		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format, format.colorSpace,
			extent, 1, vk::ImageUsageFlagBits::eColorAttachment
		);

		vkUtil::QueueFamilyIndices indices = vkUtil::FindQueueFamilies(physicalDevice, surface);
		uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		// if there are 2 seperate queue family indices
		if (indices.GraphicsFamily.value() != indices.PresentFamily.value()) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		createInfo.preTransform = support.Capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

		SwapChainBundle bundle{};
		try {
			bundle.Swapchain = logicalDevice.createSwapchainKHR(createInfo);
		}
		catch (vk::SystemError err) {
			VO_CORE_ERROR("Failed to create swapchain");
			throw std::runtime_error("failed to create swapchain");
		}

		std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(bundle.Swapchain);
		bundle.Frames.resize(images.size());

		for (size_t i = 0; i < images.size(); ++i) {
			vk::ImageViewCreateInfo createInfo = {};
			createInfo.image = images[i];
			createInfo.viewType = vk::ImageViewType::e2D;
			createInfo.components.r = vk::ComponentSwizzle::eIdentity;
			createInfo.components.g = vk::ComponentSwizzle::eIdentity;
			createInfo.components.b = vk::ComponentSwizzle::eIdentity;
			createInfo.components.a = vk::ComponentSwizzle::eIdentity;
			createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			createInfo.format = format.format;

			bundle.Frames[i].Image = images[i];
			bundle.Frames[i].ImageView = logicalDevice.createImageView(createInfo);
		}
		
		bundle.Format = format.format;
		bundle.Extent = extent;

		return bundle;
	}
}