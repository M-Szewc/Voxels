#pragma once
#include "Core/Base.h"

namespace vkInit {
	
	vk::Semaphore CreateSemaphore(vk::Device device);

	vk::Fence CreateFence(vk::Device device);

}