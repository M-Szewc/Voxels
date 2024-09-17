#include "vopch.h"

#include "VulkanUtil.h"

namespace vkUtil {

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
}