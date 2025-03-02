workspace "Pawn Engine"
	architecture "x64"
	configurations {"Debug", "Release", "Distribute"}
	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

local includeDirs = {}
includeDirs["vendor"] = "Engine/vendor/"
includeDirs["engine"] = "Engine"

require "GLFW/premake5" -- GLFW

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource (includeDirs.engine .. "/pch.cpp")

	characterset "Unicode"

	files 
	{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
		"%{prj.name}/PawnEngine.h",
		"%{prj.name}/pch.h",
		"%{prj.name}/pch.cpp",
	}

	includedirs 
	{
		includeDirs.vendor .. "/include",
		includeDirs.engine,
		includeDirs.engine .. "/Source",
	}

	defines 
	{
		"SPDLOG_BUILD_SHARED",
		"PAWN_LIBRARY_BUILD", 
		"_WINDLL",
	}

	buildoptions
	{
		"/utf-8",
	}

	filter "system:windows"
		systemversion "latest"

		postbuildcommands
		{
			("{COPYDIR} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox/")
		}

	filter "configurations:Debug"
		defines "PE_DEBUG"
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "PE_RELEASE"
		optimize "on"
		runtime "Release"

	filter "configurations:Distribute"
		defines "PE_DIST"
		optimize "on"
		runtime "Release"



project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "On"

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
		includeDirs.vendor .. "/include",
		includeDirs.engine,
		includeDirs.engine .. "/Source",
	}

	links 
	{
		"Engine",
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