#pragma once

#include <vopch.h>

namespace vkInit {

	// checks if the extensions and layers are supported
	bool AreSupported(const std::vector<const char*>& extensions, const std::vector<const char*>& layers) {
	
		//check extension support
		std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

#ifdef VO_DEBUG
		std::stringstream ss;
		ss << "Device can support the following extensions:" << std::endl;

		for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
			ss << '\t' << supportedExtension.extensionName << std::endl;
		}

		VO_CORE_TRACE(ss.str());
#endif

		// quadratic search for supported extensions
		bool found = false;
		for (const char* extension : extensions) {
			for (vk::ExtensionProperties supportedExtension : supportedExtensions) {
				if (strcmp(extension, supportedExtension.extensionName) == 0) {
					found = true;
#ifdef VO_DEBUG
					VO_CORE_INFO("Extension \"{0}\" is supported", extension);
#endif
				}
			}
			if(!found) {
#ifdef VO_DEBUG
				VO_CORE_ERROR("Extension \"{0}\" is not supported!", extension);
#endif
				return false;
			}
		}

		//check layer support
		std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();

#ifdef VO_DEBUG
		ss.str("");
		ss << "Device can support the following layers:" << std::endl;
		for (vk::LayerProperties supportedLayer : supportedLayers) {
			ss << '\t' << supportedLayer.layerName << std::endl;
		}
		VO_CORE_TRACE(ss.str());
#endif

		// quadratic search for supported layers
		for (const char* layer : layers) {
			found = false;
			for (vk::LayerProperties supportedLayer : supportedLayers) {
				if (strcmp(layer, supportedLayer.layerName) == 0) {
					found = true;
#ifdef VO_DEBUG
					VO_CORE_INFO("Layer \"{0}\" is supported", layer);
#endif
				}
			}
			if (!found) {
#ifdef VO_DEBUG
				VO_CORE_ERROR("Layer \"{0}\" is not supported!", layer);
#endif
				return false;
			}
		}

		return true;
	}

	// create Vulkan instance
	vk::Instance MakeInstance(const char* applicationName) {

#ifdef VO_DEBUG
		VO_CORE_TRACE("Creating Vulkan instance");
#endif

		// get Vulkan version
		uint32_t version{ 0 };
		vkEnumerateInstanceVersion(&version);

#ifdef VO_DEBUG
		VO_CORE_INFO("System supports Vulkan variant: {0}, Major: {1}, Minor: {2}, Patch: {3}",
			VK_API_VERSION_VARIANT(version),
			VK_API_VERSION_MAJOR(version),
			VK_API_VERSION_MINOR(version),
			VK_API_VERSION_PATCH(version));
#endif

		// ignoring Patch - using patch 0 for compatibility
		version &= ~(0xFFFU);

		// create application info
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
		extensions.push_back("VK_EXT_debug_utils");
#endif

#ifdef VO_DEBUG
		std::stringstream ss;
		ss << "extensions to be requested:" << std::endl;

		for (const char* extensionName : extensions) {
			ss << "\t\"" << extensionName << "\"" << std::endl;
		}

		VO_CORE_TRACE(ss.str());
#endif

		std::vector<const char*> layers;

		// add validation layer for debug
#ifdef VO_DEBUG
		layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

		if (!AreSupported(extensions, layers)) {
			return nullptr;
		}

		// creating creation info and passing extensions
		vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			static_cast<uint32_t>(layers.size()), layers.data(), // enabled layers
			static_cast<uint32_t>(extensions.size()), extensions.data() // enabled extensions
		);

		// try to create instance using creation info
		try {
			return vk::createInstance(createInfo, nullptr);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create Vulkan instance");
#endif
			return nullptr;
		}
	}
}