#pragma once
#include "vopch.h"

namespace vkUtil {

	struct QueueFamilyIndices {
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete() {
			return GraphicsFamily.has_value() && PresentFamily.has_value();
		}
	};

	struct SwapChainFrame {
		vk::Image Image;
		vk::ImageView ImageView;
		vk::Framebuffer FrameBuffer;
		vk::CommandBuffer CommandBuffer;
	};

	// returns queue family indices
	QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);

}