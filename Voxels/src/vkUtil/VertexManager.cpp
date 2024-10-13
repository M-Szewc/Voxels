#include "vopch.h"
#include "VertexManager.h"

namespace vkUtil {

	VertexManager::VertexManager()
		:m_Offset{ 0 } {

	}

	VertexManager::~VertexManager()
	{
		m_LogicalDevice.destroyBuffer(m_VertexBuffer.m_Buffer);
		m_LogicalDevice.freeMemory(m_VertexBuffer.m_BufferMemory);
	}

	void VertexManager::Consume(MeshType type, const std::vector<float>& vertexData)
	{
		for (float attribute : vertexData) {
			m_Lump.push_back(attribute);
		}
		int vertexCount = static_cast<int>(vertexData.size() / 5);

		m_Offsets.insert(std::make_pair(type, m_Offset));
		m_Sizes.insert(std::make_pair(type, vertexCount));

		m_Offset += vertexCount;
	}

	void VertexManager::Finalize(FinalizationChunk finalizationChunk)
	{
		this->m_LogicalDevice = finalizationChunk.LogicalDevice;

		BufferInput inputChunk;
		inputChunk.LogicalDevice = finalizationChunk.LogicalDevice;
		inputChunk.PhysicalDevice = finalizationChunk.PhysicalDevice;
		inputChunk.Size = sizeof(float) * m_Lump.size();
		inputChunk.Usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc;
		inputChunk.MemoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

		Buffer stagingBuffer = vkUtil::CreateBuffer(inputChunk);

		void* memoryLocation = m_LogicalDevice.mapMemory(stagingBuffer.m_BufferMemory, 0, inputChunk.Size);
		memcpy(memoryLocation, m_Lump.data(), inputChunk.Size);
		m_LogicalDevice.unmapMemory(stagingBuffer.m_BufferMemory);
	
		inputChunk.Usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
		inputChunk.MemoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

		m_VertexBuffer = CreateBuffer(inputChunk);

		CopyBuffer(stagingBuffer, m_VertexBuffer, inputChunk.Size,
			finalizationChunk.Queue, finalizationChunk.CommandBuffer
		);

		m_LogicalDevice.destroyBuffer(stagingBuffer.m_Buffer);
		m_LogicalDevice.freeMemory(stagingBuffer.m_BufferMemory);
	}
}