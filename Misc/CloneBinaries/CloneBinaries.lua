project "CloneBinaries"
	kind "Utility"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	postbuildmessage "Cloning binaries!"
    prebuildcommands
    {
        ("{MKDIR} %{wks.location}bin/" .. outputdir .. "/Sandbox/"),
        ("{COPY} %{wks.location}/bin/" .. outputdir .."/Engine/Engine.dll %{wks.location}/bin/" .. outputdir .. "/Sandbox"),
        ("{COPY} %{wks.location}/bin/" .. outputdir .."/EngineCore/EngineCore.dll %{wks.location}/bin/" .. outputdir .. "/Sandbox"),
    }
    postbuildcommands
	{
        ("{RMDIR} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}"),
    }
	