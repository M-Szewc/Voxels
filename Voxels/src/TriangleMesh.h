#pragma once
#include "Base.h"
#include "Memory.h"

namespace vkMesh {
	class TriangleMesh
	{
	public:
		TriangleMesh(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice);
		~TriangleMesh();
		vkUtil::Buffer m_VertexBuffer;
	private:
		vk::Device m_LogicalDevice;
	};
}

