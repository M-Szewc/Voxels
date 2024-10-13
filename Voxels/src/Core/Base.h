#pragma once

#include <memory>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <functional>
#include <optional>
#include <fstream>
#include <unordered_map>

#include <GLFW/glfw3.h>

// statically load library - doesn't have all functions
#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Assert.h"
#include "Core/Core.h"