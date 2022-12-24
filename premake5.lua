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
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Engine/vendor/GLFW/include"
IncludeDir["GLAD"] = "%{wks.location}/Engine/vendor/GLAD/include"
IncludeDir["glm"] = "%{wks.location}/Engine/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Engine/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Engine/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Engine/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/Engine/vendor/Box2D/include"
IncludeDir["mono"] = "%{wks.location}/Engine/vendor/mono/include"
IncludeDir["uuid_v4"] = "%{wks.location}/Engine/vendor/uuid_v4"
IncludeDir["OpenAl_Soft"] = "%{wks.location}/Engine/vendor/OpenAl-Soft/include"
IncludeDir["OpenAl_Soft_src"] = "%{wks.location}/Engine/vendor/OpenAl-Soft/src"

IncludeDir["nfd"] = "%{wks.location}/Editor/vendor/nfd/src/include"
IncludeDir["ImGui"] = "%{wks.location}/Editor/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Editor/vendor/ImGuizmo"

LibraryDir = { }
LibraryDir["mono"] = "%{wks.location}/vendor/lib/mono"

Library = { }
Library["mono"] = "%{LibraryDir.mono}/mono-2.0-sgen.lib"

group "Dependencies"
	include "Engine/vendor/GLFW"
	include "Engine/vendor/GLAD"
	include "Engine/vendor/yaml-cpp"
	include "Engine/vendor/Box2D"
	include "Engine/vendor/OpenAl-Soft"
	
	include "Editor/vendor/imgui"
	include "Editor/vendor/nfd"
group ""

group "Core"
	include "Engine"
	include "DE_ScriptCore"
group ""

group "Tools"
	include "Editor"
group ""