project "EngineCore"
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
		"Core/**.h",
		"Core/**.cpp",
		"Core.h",
		"pch.h",
		"pch.cpp",
	}

	includedirs 
	{
		includeDirs.EngineCore,
		includeDirs.spdlog,
	}

	defines 
	{
		"SPDLOG_BUILD_SHARED",
		"PAWN_CORE_LIBRARY_BUILD", 
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