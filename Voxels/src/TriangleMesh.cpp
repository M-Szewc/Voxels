#include "vopch.h"
#include "TriangleMesh.h"

namespace vkMesh {
	TriangleMesh::TriangleMesh(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice)
		:m_LogicalDevice(logicalDevice)
	{
		std::vector<float> vertices = { {
			 0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
			 0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
			-0.05f, 0.05f, 0.0f, 1.0f, 0.0f
		} };

		vkUtil::BufferInput inputChunk;
		inputChunk.logicalDevice = logicalDevice;
		inputChunk.physicalDevice = physicalDevice;
		inputChunk.size = sizeof(float) * vertices.size();
		inputChunk.usage = vk::BufferUsageFlagBits::eVertexBuffer;

		m_VertexBuffer = vkUtil::CreateBuffer(inputChunk);
		
		void* memoryLocation = logicalDevice.mapMemory(m_VertexBuffer.bufferMemory, 0, inputChunk.size);
		memcpy(memoryLocation, vertices.data(), inputChunk.size);
		logicalDevice.unmapMemory(m_VertexBuffer.bufferMemory);
	}

	TriangleMesh::~TriangleMesh()
	{
		m_LogicalDevice.destroyBuffer(m_VertexBuffer.buffer);
		m_LogicalDevice.freeMemory(m_VertexBuffer.bufferMemory);
	}
}
