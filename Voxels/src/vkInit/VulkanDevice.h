#pragma once
#include "Core/Base.h"

namespace vkInit {

	// checks if device supports all requested extensions
	bool CheckDeviceExtensionSupport(
		const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions
	);

	// checks if device supports extensions
	bool IsSuitable(const vk::PhysicalDevice& device);

	// returns suitable aveilable physical device
	vk::PhysicalDevice GetPhysicalDevice(vk::Instance& instance);

	// creates device - abstracted device from physical device and queue families
	vk::Device CreateLogicalDevice(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);

	// returns queue indecies 0 - graphics 1 - presenting
	std::array<vk::Queue, 2> GetQueue(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface);
}