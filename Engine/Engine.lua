project "Engine"
	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource ("pch.cpp")

	characterset "Unicode"

	files 
	{
		"Source/**.h",
		"Source/**.cpp",
		"PawnEngine.h",
		"pch.h",
		"pch.cpp",
	}

	includedirs 
	{
		"Source",
		"vendor/spdlog/include/",
		"vendor/ImGui",
		includeDirs.EngineCore,
	}

	defines 
	{
		"SPDLOG_BUILD_SHARED",
		"PAWN_LIBRARY_BUILD", 
		"_WINDLL",
	}

	links
	{
		"EngineCore",
		"ImGui",
	}

	libdirs
	{
		"%{wks.location}/bin/" .. outputdir .. "/EngineCore",
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