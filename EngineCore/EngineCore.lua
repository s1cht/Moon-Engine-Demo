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
		"PAWN_CORE_LIBRARY_BUILD", 
		"_WINDLL",
	}

	links
	{
		"xxHash",
	}

	filter "system:windows"
		systemversion "latest"

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