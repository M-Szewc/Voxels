#pragma once
#include "vopch.h"

namespace vkInit {

	struct QueueFamilyIndices {
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete() {
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		vk::SurfaceCapabilitiesKHR Capabilities;
		std::vector<vk::SurfaceFormatKHR> Formats;
		std::vector<vk::PresentModeKHR> PresentModes;
	};

	struct SwapChainBundle {
		vk::SwapchainKHR Swapchain;
		std::vector<vk::Image> Images;
		vk::Format Format;
		vk::Extent2D Extent;
	};

	// logs device properties
	void LogDeviceProperties(const vk::PhysicalDevice& device) {

		vk::PhysicalDeviceProperties properties = device.getProperties();
		
		std::stringstream ss;

		ss << "DeviceName: " << properties.deviceName << std::endl;

		switch (properties.deviceType) {
		case(vk::PhysicalDeviceType::eCpu):
			ss << "CPU" << std::endl;
			break;
		case(vk::PhysicalDeviceType::eDiscreteGpu):
			ss << "Discrete GPU" << std::endl;
			break;
		case(vk::PhysicalDeviceType::eIntegratedGpu):
			ss << "Integrated GPU" << std::endl;
			break;
		case(vk::PhysicalDeviceType::eVirtualGpu):
			ss << "Virtual GPU" << std::endl;
			break;

		default:
			ss << "Other" << std::endl;
		}

		VO_CORE_INFO(ss.str());
	}

	// checks if device supports all requested extensions
	bool CheckDeviceExtensionSupport(
		const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions
	) {
		std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());
		
#ifdef VO_DEBUG
		std::stringstream ss;
		ss << "Device can support extensions:" << std::endl;
#endif

		for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) {
#ifdef VO_DEBUG
			ss << "\t\"" << extension.extensionName << "\"" << std::endl;
#endif
			requiredExtensions.erase(extension.extensionName);
		}
	
#ifdef VO_DEBUG
		VO_CORE_INFO(ss.str());
#endif

		return requiredExtensions.empty();
	}

	// checks if device supports extensions
	bool IsSuitable(const vk::PhysicalDevice& device) {
#ifdef VO_DEBUG
		VO_CORE_TRACE("Checking if device is suitable");
#endif
		const std::vector<const char*> requestedExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};


#ifdef VO_DEBUG
		std::stringstream ss;
		ss << "Requesting device extensions:" << std::endl;

		for (const char* extension : requestedExtensions) {
			ss << "\t\"" << extension << "\"" << std::endl;
		}

		VO_CORE_TRACE(ss.str());
#endif

		if (bool extensionsSupported = CheckDeviceExtensionSupport(device, requestedExtensions)) {
#ifdef VO_DEBUG
			VO_CORE_INFO("Device can support the requested extensions");
#endif

		}
		else {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Device cannot support the requested extensions!");
#endif

			return false;
		}
		return true;
	}

	// returns suitable aveilable physical device
	vk::PhysicalDevice GetPhysicalDevice(vk::Instance& instance) {

#ifdef VO_DEBUG
		VO_CORE_TRACE("Choosing physical device...");
#endif
		
		std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

#ifdef VO_DEBUG
		VO_CORE_INFO("There {1} {0} physical device{2} available on this system", availableDevices.size(),
			(availableDevices.size()==1?"is":"are"), (availableDevices.size() == 1 ? "" : "s"));
#endif

		for (vk::PhysicalDevice device : availableDevices) {
#ifdef VO_DEBUG
			LogDeviceProperties(device);
#endif

			if (IsSuitable(device)) {
				return device;
			}
		}

		return nullptr;
	}

	// returns queue family indices
	QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

#ifdef VO_DEBUG
		VO_CORE_INFO("System can support {0} queue families", queueFamilies.size());
#endif

		int i = 0;
		for (const vk::QueueFamilyProperties& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.GraphicsFamily = i;

#ifdef VO_DEBUG
				VO_CORE_INFO("Queue Family {0} is suitable for graphics", i);
#endif
			}

			if (device.getSurfaceSupportKHR(i, surface)) {
				indices.PresentFamily = i;

#ifdef VO_DEBUG
				VO_CORE_INFO("Queue Family {0} is suitable for presenting", i);
#endif
			}

			if (indices.IsComplete()) {
				break;
			}

			++i;
		}

		return indices;
	}

	// creates device - abstracted device from physical device and queue families
	vk::Device CreateLogicalDevice(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {

		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

		std::vector<uint32_t> uniqueIndices;
		uniqueIndices.push_back(indices.GraphicsFamily.value());
		if (indices.GraphicsFamily.value() != indices.PresentFamily.value()) {
			uniqueIndices.push_back(indices.PresentFamily.value());
		}

		float queuePriority = 1.0f;

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;

		for (uint32_t queueFamilyIndex : uniqueIndices) {
			queueCreateInfo.push_back(vk::DeviceQueueCreateInfo(
				vk::DeviceQueueCreateFlags(),
				indices.GraphicsFamily.value(),
				1, &queuePriority
			));
		}
		
		// request swapchain extension
		std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

		std::vector<const char*> enabledLayers;
#ifdef VO_DEBUG
		enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

		vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			queueCreateInfo.size(), queueCreateInfo.data(),
			enabledLayers.size(), enabledLayers.data(),
			deviceExtensions.size(), deviceExtensions.data(),
			&deviceFeatures
		);

		try {
			vk::Device device = physicalDevice.createDevice(deviceInfo);
#ifdef VO_DEBUG
			VO_CORE_INFO("GPU has been successfully abstracted");
#endif

			return device;
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to abstract GPU!");
#endif

			return nullptr;
		}
	}

	// returns queue indecies 0 - graphics 1 - presenting
	std::array<vk::Queue, 2> GetQueue(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface) {
		
		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
		
		return { {
				device.getQueue(indices.GraphicsFamily.value(), 0),
				device.getQueue(indices.PresentFamily.value(), 0)
			} };
	}

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

	vk::SurfaceFormatKHR ChoostSwapchainSurfaceFormat(std::vector < vk::SurfaceFormatKHR> formats) {

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

		vk::SurfaceFormatKHR format = ChoostSwapchainSurfaceFormat(support.Formats);

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

		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
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

		bundle.Images = logicalDevice.getSwapchainImagesKHR(bundle.Swapchain);
		bundle.Format = format.format;
		bundle.Extent = extent;

		return bundle;
	}
}