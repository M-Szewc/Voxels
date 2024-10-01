#pragma once
#include "Base.h"

namespace Game {

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

