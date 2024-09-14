#pragma once

#include <vopch.h>

namespace vkInit {
	vk::Instance MakeInstance(const char* applicationName) {
#ifdef VO_DEBUG
		std::cout << "Creating Vulkan instance" << std::endl;
#endif
		uint32_t version{ 0 };
		vkEnumerateInstanceVersion(&version);

#ifdef VO_DEBUG
		std::cout << "System supports Vulkan variant: "
			<< VK_API_VERSION_VARIANT(version)
			<< ", Major: " << VK_API_VERSION_MAJOR(version)
			<< ", Minor: " << VK_API_VERSION_MINOR(version)
			<< ", Patch: " << VK_API_VERSION_PATCH(version) << std::endl;
#endif
		// ignoring Patch - using patch 0 for compatibility
		version &= ~(0xFFFU);

		vk::ApplicationInfo appInfo = vk::ApplicationInfo(
			applicationName,
			version,
			"Vulkan Voxel Vengine",
			version,
			version
		);

		// checking for glfw extensions

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef VO_DEBUG
		std::cout << "extensions to be requested:" << std::endl;

		for (const char* extensionName : extensions) {
			std::cout << "\t\"" << extensionName << "\"" << std::endl;
		}
#endif

		// creating creation info and passing extensions

		vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			0, nullptr, // enabled layers
			static_cast<uint32_t>(extensions.size()), extensions.data() // enabled extensions
		);

		// create instance using creation info

		try {
			return vk::createInstance(createInfo, nullptr);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			std::cout << "Failed to create Vulkan instance" << std::endl;
#endif
			return nullptr;
		}
	}
}