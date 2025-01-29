workspace "Pawn Engine"
	architecture "x64"
	configurations {"Debug", "Release", "Distribute"}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

local includeDirs = {}
includeDirs["vendor"] = "Engine/vendor/"
includeDirs["engine"] = "Engine"

require "GLFW/premake5" -- GLFW

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource (includeDirs.engine .. "/pch.cpp")

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

	buildoptions
	{
		"/utf-8",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "latest"

		defines 
		{
			"PAWN_LIBRARY_BUILD", 
			"_WINDLL",
		}

		postbuildcommands
		{
			("{COPYDIR} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox/")
		}

	filter "configurations:Debug"
		defines "PE_DEBUG"
		symbols "On"
		staticruntime "off"
		runtime "Debug"

	filter "configurations:Release"
		defines "PE_RELEASE"
		optimize "On"
		staticruntime "off"
		runtime "Release"

	filter "configurations:Distribute"
		defines "PE_DIST"
		optimize "On"
		staticruntime "off"
		runtime "Release"



project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

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
		cppdialect "C++20"
		staticruntime "On"
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