project "OpenAL-Soft"
	kind "StaticLib"
	language "C++"
	cppdialect "C++14"
	staticruntime "off"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"src",
		"src/alc",
		"src/common",
		"include",
		"include/AL"
	}

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	excludes
	{
		"src/alc/mixer/mixer_neon.cpp"
	}

	defines
	{
		"AL_LIBTYPE_STATIC"
	}
	
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"AL_BUILD_LIBRARY",
			"AL_ALEXT_PROTOTYPES",
			"_CRT_SECURE_NO_WARNINGS",
			"NOMINMAX",
			"OpenAL_EXPORTS"
		}

		links
		{
			"winmm"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
