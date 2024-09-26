#pragma once

#include <GLFW/glfw3.h>

// statically load library - doesn't have all functions
#include <vulkan/vulkan.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <set>
#include <string>
#include <optional>
#include <fstream>

#include <Core.h>
#include <Log.h>

#define _CRTDBG_MAP_ALLOC