include "Dependencies.lua"

workspace "Voxels"
	architecture "x64"
	startproject "Voxels"
	
	configurations
	{
		"Debug", "Release", "Distribution"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Voxels/vendor/GLFW"
group ""

include "Voxels"