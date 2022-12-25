project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir)

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
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.mono}",
		"%{IncludeDir.uuid_v4}",
		"%{IncludeDir.OpenAL_Soft}"
	}

	links
	{
		"GLFW",
		"GLAD",
		"yaml-cpp",
		"Box2D",
		"OpenAL-Soft",
		"opengl32.lib",
		"%{Library.mono}"
	}

	defines
	{
		"__ENGINE__",
		"GLFW_INCLUDE_NONE",
		"YAML_CPP_STATIC_DEFINE",
		"AL_LIBTYPE_STATIC"
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