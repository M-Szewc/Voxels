#include "vopch.h"
#include "Scene.h"

namespace Game {

	Scene::Scene() {
		for (float x = -1.0f; x < 1.0f; x += 0.2f) {
			for (float y = -1.0f; y < 1.0f; y += 0.2f) {
				m_TrianglePositions.push_back(glm::vec3(x, y, 0.0f));
			}
		}
	}

}