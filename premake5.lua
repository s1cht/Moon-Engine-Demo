include "./vendor/Premake/Customization/solutions.lua"
include "Dependencies.lua"

workspace "Moon Engine"
	architecture "x64"
	configurations {
		"Debug", 
		"Release", 
		"Distribute"
	}
	solution_items
	{
		".editorconfig"
	}

	startproject "Sandbox"

outputdir					= "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
engineVendor				= "%{wks.location}/Engine/vendor/"

require "Sandbox"
group "Dependencies"
	require "vendor/ImGui"						-- ImGui
	include "vendor/xxHash"						-- xxHash
	require "Misc/BuildDXC"						-- DXC
	require "Misc/BuildSPIRV-Tools"				-- SPIRV-Tools

group "Core"
	require "Engine"							-- Engine
	require "EngineCore"						-- EngineCore

group "Tests"
	require "Tests/CoreTest"					-- Engine core tests

group "Misc"
	require "Assets"
	include "vendor/Premake"
	require "Misc/CloneBinaries"
