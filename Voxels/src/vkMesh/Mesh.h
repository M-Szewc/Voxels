#pragma once
#include "Core/Base.h"


namespace vkMesh {

	vk::VertexInputBindingDescription GetPosColorBindingDescription();

	std::array<vk::VertexInputAttributeDescription, 2> GetPosColorAttributeDescriptions();
}

