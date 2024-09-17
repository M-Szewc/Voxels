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
}