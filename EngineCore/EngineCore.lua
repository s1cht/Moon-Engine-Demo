project "EngineCore"
	kind "SharedLib"
	language "C++"
	cppdialect "C++23"
	staticruntime "off"
	allmodulespublic "off"
	scanformoduledependencies "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	EngineCoreLib = "%{cfg.buildtarget.relpath}"

	characterset "Unicode"

	files
	{
		"Core/**.h",
		"Core/**.hpp",
		"Core/**.cpp",
		"Core/**.cppm", -- modules
		"Core.hpp",
	}

	includedirs 
	{
		includeDirs.EngineCore,
		includeDirs.spdlog,
		includeDirs.xxHash,
	}

	defines 
	{
		"SPDLOG_BUILD_SHARED",
		"MOON_CORE_LIBRARY_BUILD", 
		"_WINDLL",
	}

	links
	{
		"xxHash",
	}

	filter "system:windows"
		systemversion "latest"
		links
		{
			"shlwapi.lib",
		}

	filter "system:linux"
		toolset "clang"
		
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
		defines "ME_DEBUG"
		symbols "on"
		runtime "Debug"

	filter "configurations:Release"
		defines "ME_RELEASE"
		optimize "on"
		runtime "Release"

	filter "configurations:Distribute"
		defines "ME_DIST"
		optimize "on"
		runtime "Release"