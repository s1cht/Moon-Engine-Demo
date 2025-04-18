project "Engine"
	if string.sub(_ACTION, 1, 5) == "gmake" then
		error "GCC is not supported yet. Waiting for full C++20 modules support."
	end

	kind "SharedLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	allmodulespublic "on"
	scanformoduledependencies "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	characterset "Unicode"

	files 
	{
		"Source/**.h",
		"Source/**.cpp",
		"Source/**.cppm",
		"PawnEngine.h",
	}

	includedirs 
	{
		"Source",
		includeDirs.ImGui,
		--Vulkan SDK
		VULKAN_SDK .."/Include",

		includeDirs.spdlog,
		includeDirs.EngineCore,
		includeDirs.xxHash,
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
		"DXC",
	}

	libdirs
	{
		"%{wks.location}/bin/" .. outputdir .. "/EngineCore",
	}

	filter "system:windows"
		systemversion "latest"

		postbuildcommands
		{
			("{COPYDIR} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox/")
		}

		includedirs
		{
			"C:/Windows/System32",
		}

		libdirs
		{
			VULKAN_SDK .."/Lib",
		}

		links
		{
			"vulkan-1",
			"d3d11",
			"d3d12",
			"dxgi",
		}

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