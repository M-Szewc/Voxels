#include "vopch.h"

#include "Sync.h"

namespace vkInit {

	vk::Semaphore CreateSemaphore(vk::Device device) {
		vk::SemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.flags = vk::SemaphoreCreateFlags();

		try {
			return device.createSemaphore(semaphoreInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create semaphore");
#endif
			return nullptr;
		}
	}

	vk::Fence CreateFence(vk::Device device) {
		vk::FenceCreateInfo fenceInfo = {};
		fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

		try {
			return device.createFence(fenceInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create fence");
#endif
			return nullptr;
		}
	}
}