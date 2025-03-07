engineVendor				= "%{wks.location}/Engine/vendor/"

requireDirs					= {}
requireDirs["EngineCore"]	= "%{wks.location}/EngineCore"
requireDirs["Engine"]		= "%{wks.location}/Engine"
requireDirs["GLFW"]			= "%{wks.location}/GLFW"
requireDirs["ImGui"]		= engineVendor .. "ImGui"

includeDirs					= {}
includeDirs["EngineCore"]	= "%{wks.location}/EngineCore"
includeDirs["Engine"]		= "%{wks.location}/Engine"
includeDirs["EngineSrc"]	= "%{wks.location}/Engine/Source"
includeDirs["spdlog"]		= engineVendor .. "spdlog/include"
includeDirs["ImGui"]		= engineVendor .. "ImGui"