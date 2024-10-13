project "Voxels"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "vopch.h"
	pchsource "src/vopch.cpp"

	files
	{
		"vopch.h",
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		".",
		"src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.spdlog}"
	}

	libdirs
	{
		"%{VULKAN_SDK}/Lib"
	}

	links
	{
		"GLFW",
		"vulkan-1"
	}

	defines "GLFW_INCLUDE_VULKAN"

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
		
		defines
		{
			"VO_PLATFORM_WINDOWS",
			"VO_BUILD_DLL"
		}	

		postbuildcommands
		{
			("{COPYFILE} %{wks.location}bin/" .. outputdir .. "/Voxels %{wks.location}bin/" .. outputdir .. "/Sandbox"),
			("call %{wks.location}Voxels/shaders/shader_compile.bat %{wks.location}Voxels/shaders"),
			("{COPYFILE} %{wks.location}Voxels/shaders/compiled %{wks.location}Sandbox/shaders/compiled")
		}

	filter "configurations:Debug"
		defines "VO_DEBUG"
		staticruntime "off"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VO_RELEASE"
		staticruntime "off"
		runtime "Release"
		optimize "on"

	filter "configurations:Distribution"
		defines "VO_DISTRIBUTION"
		staticruntime "off"
		runtime "Release"
		optimize "on"