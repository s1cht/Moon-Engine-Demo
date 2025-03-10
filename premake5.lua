include "./vendor/Premake/Customization/solutions.lua"
include "Dependencies.lua"

workspace "Pawn Engine"
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




group "Dependencies"
	include "vendor/Premake"
	require "Engine/vendor/ImGui" -- ImGui

group "Core"
	require "Engine"-- Engine
	require "EngineCore"-- Engine
	
group "Misc"
	project "Sandbox"
		location "Sandbox"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
		staticruntime "off"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		characterset "Unicode"

		files 
		{
			"%{prj.name}/Source/**.h",
			"%{prj.name}/Source/**.cpp",
		}

		includedirs 
		{
			includeDirs.Engine,
			includeDirs.EngineSrc,
			includeDirs.EngineCore,
			includeDirs.spdlog,
			includeDirs.ImGui,
		}

		links 
		{
			"Engine",
			"EngineCore",
		}

		libdirs
		{
			"%{wks.location}/bin/" .. outputdir .. "/Engine",
		}

		buildoptions
		{
			"/utf-8",
		}

		filter "system:windows"
			systemversion "latest"


		filter "configurations:Debug"
			defines "PE_DEBUG"
			symbols "On"

		filter "configurations:Release"
			defines "PE_RELEASE"
			optimize "On"

		filter "configurations:Distribute"
			defines "PE_DIST"
			optimize "On"