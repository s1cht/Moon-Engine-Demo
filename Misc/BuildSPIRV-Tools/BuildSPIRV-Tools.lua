project "BuildSPIRV-Tools"
	kind "Utility"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    postbuildmessage "Building SPIRV-Tools!"

    local toolsBuild = "%{wks.location}/bin-int/" .. outputdir .. "/SPIRV-Tools" 
    local toolsBin = "%{wks.location}/bin/" .. outputdir .. "/SPIRV-Tools"
    local toolsSource = "%{wks.location}/vendor/SPIRV-Tools" 
    local config

    local function Build(config)
        return {
            "cmake " .. toolsSource ..
                " -B " .. toolsBuild ..
                " -DCMAKE_BUILD_TYPE=" .. config ..
                " -DSPIRV_TOOLS_BUILD_STATIC=ON" ..
                "" ..
                " -G Ninja",

            "cmake --build " .. toolsBuild .. " --config " .. config,    
            
            "{MKDIR} " .. toolsBin,   
            "{COPYFILE} " .. toolsBuild .. "/source/*.pdb " .. toolsBin .. "/",
            "{COPYFILE} " .. toolsBuild .. "/source/*.lib " .. toolsBin .. "/",
            "{COPYFILE} " .. toolsBuild .. "/source/*.dll " .. toolsBin .. "/",
        }
    end

    filter "configurations:Debug"
        prebuildcommands(Build("Debug"))
        
    filter "configurations:Release"
        prebuildcommands(Build("Release"))
        
    filter "configurations:Distribute"
        prebuildcommands(Build("Release"))


    postbuildcommands
    {
        ("{RMDIR} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}"),
        ("{RMDIR} %{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}"),
    }