#pragma once
#include "Base.h"
#include "Log.h"

namespace vkInit {

	struct DescriptorSetLayoutData {
		int Count;
		std::vector<int> Indices;
		std::vector<vk::DescriptorType> Types;
		std::vector<int> Counts;
		std::vector<vk::ShaderStageFlags> Stages;
	};

	vk::DescriptorSetLayout CreateDescriptorSetLayout(vk::Device device, const DescriptorSetLayoutData& bindings) {

		std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
		layoutBindings.reserve(bindings.Count);

		for (int i = 0; i < bindings.Count; ++i) {

			vk::DescriptorSetLayoutBinding layoutBinding;
			layoutBinding.binding = bindings.Indices[i];
			layoutBinding.descriptorType = bindings.Types[i];
			layoutBinding.descriptorCount = bindings.Counts[i];
			layoutBinding.stageFlags = bindings.Stages[i];
			layoutBindings.push_back(layoutBinding);
		}

		vk::DescriptorSetLayoutCreateInfo layoutInfo;
		layoutInfo.flags = vk::DescriptorSetLayoutCreateFlagBits();
		layoutInfo.bindingCount = bindings.Count;
		layoutInfo.pBindings = layoutBindings.data();

		try {
			return device.createDescriptorSetLayout(layoutInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create Descriptor Set Layout");
#endif
			return nullptr;
		}
	}

	vk::DescriptorPool CreateDescriptorPool(vk::Device device, uint32_t size, const DescriptorSetLayoutData& bindings) {

		std::vector<vk::DescriptorPoolSize> poolSizes;

		for (int i = 0; i < bindings.Count; ++i) {
			vk::DescriptorPoolSize poolSize;
			poolSize.type = bindings.Types[i];
			poolSize.descriptorCount = size;
			poolSizes.push_back(poolSize);
		}

		vk::DescriptorPoolCreateInfo poolInfo;
		poolInfo.flags = vk::DescriptorPoolCreateFlags();
		poolInfo.maxSets = size;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();

		try {
			return device.createDescriptorPool(poolInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to make descriptor pool");
#endif
			return nullptr;
		}


	}

	vk::DescriptorSet AllocateDescriptorSet(vk::Device device, vk::DescriptorPool descriptorPool, vk::DescriptorSetLayout layout) {
		
		vk::DescriptorSetAllocateInfo allocationInfo;
		allocationInfo.descriptorPool = descriptorPool;
		allocationInfo.descriptorSetCount = 1;
		allocationInfo.pSetLayouts = &layout;

		try {
			return device.allocateDescriptorSets(allocationInfo)[0];
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to allocate descriptor set from pool");
#endif
			return nullptr;
		}
	}

}