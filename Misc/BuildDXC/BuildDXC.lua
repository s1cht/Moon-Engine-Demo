project "BuildDXC"
	kind "Utility"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    postbuildmessage "Building DirectXShaderCompiler!"

    local dxcCache = "%{wks.location}/vendor/DXC/cmake/caches/PredefinedParams.cmake"
    local dxcBuild = "%{wks.location}/bin-int/" .. outputdir .. "/DXC" 
    local dxcBin = "%{wks.location}/bin/" .. outputdir .. "/DXC"
    local dxcSource = "%{wks.location}/vendor/DXC" 
    local config

    local function BuildDXC(config)
        return {
            "cmake " .. dxcSource ..
                " -B " .. dxcBuild ..
                " -C " .. dxcCache ..
                " -DCMAKE_BUILD_TYPE=" .. config ..
                " -G Ninja",

            "ninja -C " .. dxcBuild,    
            
            "{MKDIR} " .. dxcBin,   
            "{COPYFILE} " .. dxcBuild .. "/bin/*.* " .. dxcBin .. "/",
            "{COPYFILE} " .. dxcBuild .. "/lib/*.* " .. dxcBin .. "/"
        }
    end

    filter "configurations:Debug"
        prebuildcommands(BuildDXC("Debug"))
        
    filter "configurations:Release"
        prebuildcommands(BuildDXC("RelWithDebInfo"))
        
    filter "configurations:Distribute"
        prebuildcommands(BuildDXC("MinSizeRel"))


    postbuildcommands
    {
        ("{RMDIR} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}"),
        ("{RMDIR} %{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}"),
    }