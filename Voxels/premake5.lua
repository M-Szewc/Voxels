project "Voxels"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "vopch.h"
	pchsource "src/vopch.cpp"

	files
	{
		

		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		".",
		"src"
	}

	filter "system:windows"
		systemversion "latest"

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