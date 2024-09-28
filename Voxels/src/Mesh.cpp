#include "vopch.h"
#include "Mesh.h"

namespace vkMesh {

	vk::VertexInputBindingDescription GetPosColorBindingDescription() {

		vk::VertexInputBindingDescription bindingDescription;
		bindingDescription.binding = 0;
		bindingDescription.stride = 5 * sizeof(float);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;

		return bindingDescription;
	}

	std::array<vk::VertexInputAttributeDescription, 2> GetPosColorAttributeDescriptions() {

		std::array<vk::VertexInputAttributeDescription, 2> attributes;

		//Pos
		attributes[0].binding = 0;
		attributes[0].location = 0;
		attributes[0].format = vk::Format::eR32G32Sfloat;
		attributes[0].offset = 0;

		//Color
		attributes[1].binding = 0;
		attributes[1].location = 1;
		attributes[1].format = vk::Format::eR32G32B32Sfloat;
		attributes[1].offset = 2 * sizeof(float);

		return attributes;
	}
}