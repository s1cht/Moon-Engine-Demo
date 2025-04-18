include "./vendor/Premake/Customization/solutions.lua"
include "./vendor/premake-cmake/cmake.lua"
include "Dependencies.lua"

premake.path = premake.path .. ";%{wks.location}vendor/DXC/"

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
	require "Misc/CloneAssets"
	project "Sandbox"

		if string.sub(_ACTION, 1, 5) == "gmake" then
			error "GCC is not supported yet. Waiting for full C++20 modules support."
		end

		location "Sandbox"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
		staticruntime "off"
		allmodulespublic "on"
		scanformoduledependencies "on"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

		characterset "Unicode"

		files 
		{
			"%{prj.name}/Source/**.h",
			"%{prj.name}/Source/**.cpp",
			"%{prj.name}/Source/**.cppm",
		}

		includedirs 
		{
			includeDirs.Engine,
			includeDirs.EngineSrc,
			includeDirs.EngineCore,
			includeDirs.spdlog,
			includeDirs.ImGui,
			includeDirs.xxHash,
		}

		links 
		{
			"Engine",
			"EngineCore",
			"ImGui",
		}

		libdirs
		{
			"%{wks.location}/bin/" .. outputdir .. "/Engine",
		}

		filter "system:windows"
			systemversion "latest"

		filter "action:vs*"
			buildoptions
			{
				"/utf-8",
			}

		filter "action:clang*"
			buildoptions { "-fmodules-ts", "-fimplicit-modules", "-fimplicit-module-maps" }

		filter "files:**.cppm"
			compileas "Module"

		filter "files:**.cpp"
			compileas "C++"

		filter "configurations:Debug"
			defines "PE_DEBUG"
			symbols "On"

		filter "configurations:Release"
			defines "PE_RELEASE"
			optimize "On"

		filter "configurations:Distribute"
			defines "PE_DIST"
			optimize "On"