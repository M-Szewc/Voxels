workspace "Voxels"
	architecture "x64"
	startproject "Voxels"
	
	configurations
	{
		"Debug", "Release", "Distribution"
	}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Voxels"