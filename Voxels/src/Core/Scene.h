#pragma once
#include "Core/Base.h"

namespace VoxelEngine {

	class Scene
	{
	public:
		Scene();
	public:
		std::vector<glm::vec3> m_TrianglePositions;
		std::vector<glm::vec3> m_SquarePositions;
		std::vector<glm::vec3> m_PentagonPositions;
	};
}

