project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{wks.location}/Voxels/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.spdlog}"
	}

	links
	{
		"Voxels"
	}

	defines "GLFW_INCLUDE_VULKAN"

	filter "system:windows"
		systemversion "latest"
		defines "VO_PLATFORM_WINDOWS"

	filter "configurations:Debug"
		defines "VO_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "VO_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Distribution"
		defines "VO_DISTRIBUTION"
		runtime "Release"
		optimize "on"