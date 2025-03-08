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
		"C:/Windows/System32",
		"C:/VulkanSDK/1.4.304.1/Include",
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

		links
		{
			"vulkan-1.dll",
			"d3d11",
			"d3d12",
			"dxgi",
			"d3dcompiler",
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