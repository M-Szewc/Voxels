#pragma once
#include "vopch.h"

namespace vkUtil {

	std::vector<char> ReadFile(std::string filename) {

		std::ifstream file(filename, std::ios::ate | std::ios::binary);

#ifdef VO_DEBUG
		if (!file.is_open()) {
			VO_CORE_ERROR("Failed to load \"{0}\"", filename);
		}
#endif

		size_t filesize{ static_cast<size_t>(file.tellg()) };

		std::vector<char> buffer(filesize);
		file.seekg(0);
		file.read(buffer.data(), filesize);

		file.close();
		return buffer;
	}

	vk::ShaderModule CreateModule(std::string filename, vk::Device device) {

		std::vector<char> sourceCode = ReadFile(filename);

		vk::ShaderModuleCreateInfo moduleInfo = {};
		moduleInfo.flags = vk::ShaderModuleCreateFlags();
		moduleInfo.codeSize = sourceCode.size();
		moduleInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCode.data());
	
		try {
			return device.createShaderModule(moduleInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create shader module for \"{0}\"", filename);
#endif
		}
	}
}