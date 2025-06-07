	project "Sandbox"
		location "Sandbox"
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
			"Source/**.h",
			"Source/**.cpp",
			"Source/**.cppm",
		}

		includedirs 
		{
			includeDirs.Engine,
			includeDirs.EngineSrc,
			includeDirs.EngineCore,
			includeDirs.spdlog,
			includeDirs.ImGui,
			includeDirs.xxHash,
			includeDirs.DXC,
			VULKAN_SDK .."/Include",
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

		postbuildcommands
		{
			("{COPYDIR} %{wks.location}/bin/" .. outputdir .. "/EngineCore/EngineCore.dll %{wks.location}/bin/" .. outputdir .. "/Sandbox/"),
			("{COPYDIR} %{wks.location}/bin/" .. outputdir .. "/Engine/Engine.dll %{wks.location}/bin/" .. outputdir .. "/Sandbox/")
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