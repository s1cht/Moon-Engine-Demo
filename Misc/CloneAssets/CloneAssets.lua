project "CloneAssets"
	kind "Utility"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	postbuildmessage "Cloning assets!"
	prebuildcommands
	{
		("{RMDIR} %{wks.location}/bin/" .. outputdir .. "/Sandbox/assets"),
		("{MKDIR} %{wks.location}bin/" .. outputdir .. "/Sandbox/assets"),
		("{ECHO} %{wks.location}bin/" .. outputdir .. "/Sandbox/assets"),
		("{COPYDIR} %{wks.location}/Assets %{wks.location}bin/" .. outputdir .. "/Sandbox/assets"),
	}
	postbuildcommands
	{
	    ("{RMDIR} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}"),
	}
	