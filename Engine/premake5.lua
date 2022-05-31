project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "depch.h"
	pchsource "src/depch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}"
	}

	links
	{
		"GLFW",
		"GLAD",
		"ImGui",
		"yaml-cpp",
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