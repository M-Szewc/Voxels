#include "vopch.h"
#include "VertexManager.h"

namespace vkUtil {

	VertexManager::VertexManager()
		:m_Offset{ 0 } {

	}

	VertexManager::~VertexManager()
	{
		m_LogicalDevice.destroyBuffer(m_VertexBuffer.buffer);
		m_LogicalDevice.freeMemory(m_VertexBuffer.bufferMemory);
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
		this->m_LogicalDevice = finalizationChunk.logicalDevice;

		BufferInput inputChunk;
		inputChunk.logicalDevice = finalizationChunk.logicalDevice;
		inputChunk.physicalDevice = finalizationChunk.physicalDevice;
		inputChunk.size = sizeof(float) * m_Lump.size();
		inputChunk.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferSrc;
		inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;

		Buffer stagingBuffer = vkUtil::CreateBuffer(inputChunk);

		void* memoryLocation = m_LogicalDevice.mapMemory(stagingBuffer.bufferMemory, 0, inputChunk.size);
		memcpy(memoryLocation, m_Lump.data(), inputChunk.size);
		m_LogicalDevice.unmapMemory(stagingBuffer.bufferMemory);
	
		inputChunk.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
		inputChunk.memoryProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;

		m_VertexBuffer = CreateBuffer(inputChunk);

		CopyBuffer(stagingBuffer, m_VertexBuffer, inputChunk.size,
			finalizationChunk.queue, finalizationChunk.commandBuffer
		);

		m_LogicalDevice.destroyBuffer(stagingBuffer.buffer);
		m_LogicalDevice.freeMemory(stagingBuffer.bufferMemory);
	}
}