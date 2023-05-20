include "./vendor/premake/config/solution_items.lua"

workspace "DragonEngine"
	architecture "x64"
	startproject "Editor"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	
	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

-- Include directories relative to root folder (solution directory)
IncludeDir = { }

-- Engine Includes
IncludeDir["spdlog"] = "%{wks.location}/Engine/vendor/modules/spdlog/include"
IncludeDir["glfw"] = "%{wks.location}/Engine/vendor/modules/glfw/include"
IncludeDir["glad"] = "%{wks.location}/Engine/vendor/modules/glad/include"
IncludeDir["glm"] = "%{wks.location}/Engine/vendor/modules/glm"
IncludeDir["entt"] = "%{wks.location}/Engine/vendor/modules/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Engine/vendor/modules/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/Engine/vendor/modules/Box2D/include"
IncludeDir["mono"] = "%{wks.location}/Engine/vendor/modules/mono/include"
IncludeDir["uuid_v4"] = "%{wks.location}/Engine/vendor/modules/uuid_v4"
IncludeDir["OpenAL_Soft"] = "%{wks.location}/Engine/vendor/modules/OpenAL-Soft/include"
IncludeDir["vulkan"] = "%{wks.location}/Engine/vendor/modules/vulkan/include"

-- Editor Includes
IncludeDir["stb_image"] = "%{wks.location}/Editor/vendor/modules/stb_image"
IncludeDir["nfd"] = "%{wks.location}/Editor/vendor/modules/nfd/src/include"
IncludeDir["ImGui"] = "%{wks.location}/Editor/vendor/modules/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Editor/vendor/modules/ImGuizmo"
IncludeDir["AudioToWav"] = "%{wks.location}/Editor/vendor/modules/AudioToWav/include"
IncludeDir["IconFontCppHeaders"] = "%{wks.location}/Editor/vendor/modules/IconFontCppHeaders"

LibraryDir = { }
LibraryDir["mono"] = "%{wks.location}/Engine/vendor/modules/mono/bin"
LibraryDir["vulkan"] = "%{wks.location}/Engine/vendor/modules/vulkan/bin"

Library = { }
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"
Library["vulkan"] = "%{LibraryDir.vulkan}/vulkan.lib"

group "Dependencies"
	include "Engine/vendor/modules/glfw"
	include "Engine/vendor/modules/glad"
	include "Engine/vendor/modules/yaml-cpp"
	include "Engine/vendor/modules/Box2D"
	include "Engine/vendor/modules/OpenAL-Soft"
	
	include "Editor/vendor/modules/imgui"
	include "Editor/vendor/modules/nfd"
	include "Editor/vendor/modules/AudioToWav"
group ""

group "Core"
	include "Engine"
	include "DE_ScriptCore"
group ""

group "Tools"
	include "Editor"
group ""