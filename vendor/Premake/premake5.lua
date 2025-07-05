project "Premake"
	kind "Utility"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{wks.location}/**premake5.lua",
		"%{wks.location}/**.lua",
	}

	prebuildmessage "Regenerating project files with Premake5!"
	prebuildcommands
	{
		"\"%{prj.location}/Binaries/premake5\" %{_ACTION} --file=\"%{wks.location}premake5.lua\""
	}
	postbuildcommands
	{
        ("{RMDIR} %{wks.location}/bin/" .. outputdir .. "/%{prj.name}"),
    }
	