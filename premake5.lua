workspace "DragonEngine"
	architecture "x64"
	startproject "Editor"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	-- Uncomment when using dll
	--[[
	defines
	{
		"ENGINE_DYNAMIC_LINK"
	}]]

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "Engine/vendor/GLFW/include"
IncludeDir["GLAD"] = "Engine/vendor/GLAD/include"
IncludeDir["ImGui"] = "Engine/vendor/imgui"
IncludeDir["glm"] = "Engine/vendor/glm"
IncludeDir["stb_image"] = "Engine/vendor/stb_image"
IncludeDir["entt"] = "Engine/vendor/entt/include"

group "Dependencies"
	include "Engine/vendor/GLFW"
	include "Engine/vendor/GLAD"
	include "Engine/vendor/imgui"
group ""

project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "depch.h"
	pchsource "Engine/src/depch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"GLFW",
		"GLAD",
		"ImGui",
		"opengl32.lib"
	}

	defines
	{
		"__ENGINE__",
		"GLFW_INCLUDE_NONE"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ENGINE_BUILD_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_BUILD_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ENGINE_BUILD_DIST"
		runtime "Release"
		optimize "on"

project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"Engine"
	}

	defines
	{
		"__EDITOR__"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ENGINE_BUILD_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_BUILD_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ENGINE_BUILD_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"Engine"
	}

	defines
	{
		"__SANDBOX__"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ENGINE_BUILD_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENGINE_BUILD_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ENGINE_BUILD_DIST"
		runtime "Release"
		optimize "on"