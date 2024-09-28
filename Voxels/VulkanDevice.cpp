#include "vopch.h"

#include "VulkanDevice.h"
#include "VulkanUtil.h"
#include "VulkanLogger.h"

namespace vkInit {

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

	vk::PhysicalDevice GetPhysicalDevice(vk::Instance& instance) {

#ifdef VO_DEBUG
		VO_CORE_TRACE("Choosing physical device...");
#endif

		std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

#ifdef VO_DEBUG
		VO_CORE_INFO("There {1} {0} physical device{2} available on this system", availableDevices.size(),
			(availableDevices.size() == 1 ? "is" : "are"), (availableDevices.size() == 1 ? "" : "s"));
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

	vk::Device CreateLogicalDevice(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {

		vkUtil::QueueFamilyIndices indices = vkUtil::FindQueueFamilies(physicalDevice, surface);

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

	std::array<vk::Queue, 2> GetQueue(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface) {

		vkUtil::QueueFamilyIndices indices = vkUtil::FindQueueFamilies(physicalDevice, surface);

		return { {
				device.getQueue(indices.GraphicsFamily.value(), 0),
				device.getQueue(indices.PresentFamily.value(), 0)
			} };
	}
}