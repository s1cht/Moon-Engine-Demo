vendor						= "%{wks.location}/vendor/"

VULKAN_SDK					= os.getenv("VULKAN_SDK")

requireDirs					= {}
requireDirs["EngineCore"]	= "%{wks.location}/EngineCore"
requireDirs["Engine"]		= "%{wks.location}/Engine"
requireDirs["ImGui"]		= vendor .. "ImGui"

includeDirs					= {}
includeDirs["EngineCore"]	= "%{wks.location}/EngineCore"
includeDirs["Engine"]		= "%{wks.location}/Engine"
includeDirs["EngineSrc"]	= "%{wks.location}/Engine/Source"
includeDirs["spdlog"]		= vendor .. "spdlog/include"
includeDirs["ImGui"]		= vendor .. "ImGui"
includeDirs["xxHash"]		= vendor .. "xxHash"
includeDirs["DXC"]		    = vendor .. "DXC/include"