#pragma once
#include "Base.h"
#include "vkMesh/MeshTypes.h"
#include "vkUtil/Memory.h"

namespace vkUtil {
	struct FinalizationChunk {
		vk::Device logicalDevice;
		vk::PhysicalDevice physicalDevice;
		vk::Queue queue;
		vk::CommandBuffer commandBuffer;
	};

	class VertexManager {
	public:
		VertexManager();
		~VertexManager();
		void Consume(MeshType type, const std::vector<float>& vertexData);
		void Finalize(FinalizationChunk finalizationChunk);
	public:
		Buffer m_VertexBuffer;
		std::unordered_map<MeshType, int> m_Offsets;
		std::unordered_map<MeshType, int> m_Sizes;
	private:
		int m_Offset;
		vk::Device m_LogicalDevice;
		std::vector<float> m_Lump;
	};
}