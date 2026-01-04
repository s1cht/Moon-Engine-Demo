project "CoreTest"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++23"
	staticruntime "off"
	allmodulespublic "off"
	scanformoduledependencies "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	characterset "Unicode"

	files 
	{
		"main.cpp",
	}

	includedirs 
	{
		includeDirs.EngineCore,
		includeDirs.spdlog,
		includeDirs.xxHash,
	}

	links 
	{
		"EngineCore",
	}

	libdirs
	{
		"%{wks.location}/bin/" .. outputdir .. "/CoreTest",
	}

	prebuildcommands
	{
		("{COPYDIR} %{wks.location}bin/" .. outputdir .. "/EngineCore/EngineCore.dll %{wks.location}bin/" .. outputdir .. "/CoreTest/"),
	}

	filter "system:windows"
		systemversion "latest"

	filter "action:vs*"
		buildoptions
		{
			"/utf-8",
			"/Fo $(IntDir)%(RelativeDir)\\",
		}

	filter "action:clang*"
		buildoptions { "-fmodules-ts", "-fimplicit-modules", "-fimplicit-module-maps" }

	filter "files:**.cppm"
		compileas "Module"

	filter "files:**.cpp"
		compileas "C++"

	filter "configurations:Debug"
		defines "ME_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "ME_RELEASE"
		optimize "On"

	filter "configurations:Distribute"
		defines "ME_DIST"
		optimize "On"