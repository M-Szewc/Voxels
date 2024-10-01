#pragma once
#include "Base.h"


namespace vkMesh {

	vk::VertexInputBindingDescription GetPosColorBindingDescription();

	std::array<vk::VertexInputAttributeDescription, 2> GetPosColorAttributeDescriptions();
}

