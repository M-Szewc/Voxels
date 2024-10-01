#include "vopch.h"

#include "VulkanLogger.h"

namespace vkInit{

	VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	) {
		if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			VO_VULKAN_FATAL(pCallbackData->pMessage);
		}
		else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			VO_VULKAN_ERROR(pCallbackData->pMessage);
		}
		else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			VO_VULKAN_WARN(pCallbackData->pMessage);
		}
		else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			VO_VULKAN_INFO(pCallbackData->pMessage);
		}
		else {
			VO_VULKAN_TRACE(pCallbackData->pMessage);
		}

		return VK_FALSE;
	}

	vk::DebugUtilsMessengerEXT CreateDebugMessanger(vk::Instance& instance, vk::DispatchLoaderDynamic& dldi) {

		// make create info

		vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			debugCallback,
			nullptr
		);

		return instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dldi);
	}

	void LogDeviceProperties(const vk::PhysicalDevice& device) {

		vk::PhysicalDeviceProperties properties = device.getProperties();

		std::stringstream ss;

		ss << "DeviceName: " << properties.deviceName << std::endl;

		switch (properties.deviceType) {
		case(vk::PhysicalDeviceType::eCpu):
			ss << "CPU" << std::endl;
			break;
		case(vk::PhysicalDeviceType::eDiscreteGpu):
			ss << "Discrete GPU" << std::endl;
			break;
		case(vk::PhysicalDeviceType::eIntegratedGpu):
			ss << "Integrated GPU" << std::endl;
			break;
		case(vk::PhysicalDeviceType::eVirtualGpu):
			ss << "Virtual GPU" << std::endl;
			break;

		default:
			ss << "Other" << std::endl;
		}

		VO_CORE_INFO(ss.str());
	}

	std::vector<std::string> TransformBitsToString(vk::SurfaceTransformFlagsKHR bits) {
		std::vector<std::string> result;

		if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
			result.push_back("identity");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90) {
			result.push_back("90 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180) {
			result.push_back("180 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270) {
			result.push_back("270 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) {
			result.push_back("horizontal mirror");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) {
			result.push_back("horizontal mirror, then 90 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) {
			result.push_back("horizontal mirror, then 180 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270) {
			result.push_back("horizontal mirror, then 270 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit) {
			result.push_back("inherited");
		}

		return result;
	}

	std::vector<std::string> AlphaCompositeBitsToString(vk::CompositeAlphaFlagsKHR bits) {
		std::vector<std::string> result;

		if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque) {
			result.push_back("opaque (alpha ignored)");
		}
		if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
			result.push_back("pre multiplied (alpha expected to already be multiplied in image)");
		}
		if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) {
			result.push_back("post multiplied (alpha will be applied during composition)");
		}
		if (bits & vk::CompositeAlphaFlagBitsKHR::eInherit) {
			result.push_back("inherited");
		}

		return result;
	}

	std::vector<std::string> ImageUsageBitsToString(vk::ImageUsageFlags bits) {
		std::vector<std::string> result;

		if (bits & vk::ImageUsageFlagBits::eTransferSrc) {
			result.push_back("transfer src: image can be used as the source of a transfer command.");
		}
		if (bits & vk::ImageUsageFlagBits::eTransferDst) {
			result.push_back("transfer dst: image can be used as the destination of a transfer command.");
		}
		if (bits & vk::ImageUsageFlagBits::eSampled) {
			result.push_back("sampled: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot either of type VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE or \
VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and be sampled by a shader");
		}
		if (bits & vk::ImageUsageFlagBits::eStorage) {
			result.push_back("storage: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot of type VK_DESCRIPTOR_STORAGE_IMAGE");
		}
		if (bits & vk::ImageUsageFlagBits::eColorAttachment) {
			result.push_back("color attachment: image can be used to create a VkImageView suitable for use as \
a color or resolve attachment in a VkFrameBuffer");
		}
		if (bits & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
			result.push_back("depth/stencil attachment: image can be used to create a VkImageView \
suitable for use as a depth/stencil or depth/stencil resolve attachment in a VkFrameBuffer");
		}
		if (bits & vk::ImageUsageFlagBits::eTransientAttachment) {
			result.push_back("transient attachment: implementations may support using memory allocations \
with the VK_MEMORY_LAZILY_ALLOCATED_BIT to back an image with this usage. This \
bit can be set for any image that can be used to create a VkImageView suitable for use as \
a color, resolve, depth/stencil/ or input attachment");
		}
		if (bits & vk::ImageUsageFlagBits::eInputAttachment) {
			result.push_back("input attachment: image can be used to create a VkImageView suitable for \
occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; be read from \
a shader as an input attacment and be used as an input attachment in a framebuffer");
		}
		if (bits & vk::ImageUsageFlagBits::eFragmentDensityMapEXT) {
			result.push_back("fragment density map: image can be used to create a VkImageView suitable \
for use as a fragment density map image");
		}
		if (bits & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR) {
			result.push_back("fragment shading rate attachment: image can be used to create a VkImageView \
suitable for use as a fragment shading rate attachment or shading rate image");
		}

		return result;
	}

	std::string PresentModeInfo(vk::PresentModeKHR presentMode) {
		return vk::to_string(presentMode);
	}
}