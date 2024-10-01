#include "vopch.h"
#include "Scene.h"

namespace Game {

	Scene::Scene() {
		for (float y = -1.0f; y < 1.0f; y += 0.2f) {
			m_TrianglePositions.push_back(glm::vec3(-0.6f, y, 0.0f));
		}

		for (float y = -1.0f; y < 1.0f; y += 0.2f) {
			m_SquarePositions.push_back(glm::vec3(0.0f, y, 0.0f));
		}

		for (float y = -1.0f; y < 1.0f; y += 0.2f) {
			m_PentagonPositions.push_back(glm::vec3(0.6f, y, 0.0f));
		}

	}

}