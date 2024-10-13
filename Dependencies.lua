VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Voxels/vendor/GLFW/include"
IncludeDir["Vulkan"] = "%{VULKAN_SDK}/include"
IncludeDir["spdlog"] = "%{wks.location}/Voxels/vendor/spdlog/include"