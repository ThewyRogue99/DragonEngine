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

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Engine/vendor/GLFW/include"
IncludeDir["GLAD"] = "%{wks.location}/Engine/vendor/GLAD/include"
IncludeDir["ImGui"] = "%{wks.location}/Engine/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Engine/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Engine/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Engine/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Engine/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Engine/vendor/ImGuizmo"
IncludeDir["Box2D"] = "%{wks.location}/Engine/vendor/Box2D/include"
IncludeDir["mono"] = "%{wks.location}/Engine/vendor/mono/include"

LibraryDir = {}
LibraryDir["mono"] = "%{wks.location}/Engine/vendor/mono/lib"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/mono-2.0-sgen.lib"

group "Dependencies"
	include "Engine/vendor/GLFW"
	include "Engine/vendor/GLAD"
	include "Engine/vendor/imgui"
	include "Engine/vendor/yaml-cpp"
	include "Engine/vendor/Box2D"
group ""

group "Core"
	include "Engine"
	include "DE_ScriptCore"
group ""

group "Tools"
	include "Editor"
group ""