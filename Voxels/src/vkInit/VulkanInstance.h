#pragma once

#include "Base.h"

namespace vkInit {

	// checks if the extensions and layers are supported
	bool AreSupported(const std::vector<const char*>& extensions, const std::vector<const char*>& layers);

	// create Vulkan instance
	vk::Instance MakeInstance(const char* applicationName);
}