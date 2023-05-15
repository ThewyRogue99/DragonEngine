project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	modules_dir = "vendor/modules"

	targetdir ("%{wks.location}/bin/" .. outputdir)
	objdir ("%{wks.location}/bin-int/" .. outputdir)

	files
	{
		"src/**.h",
		"src/**.cpp",
		"%{modules_dir}/ImGui/backends/imgui_impl_glfw.cpp",
		"%{modules_dir}/ImGuizmo/ImGuizmo.cpp",

        -- If using OpenGL
		"%{modules_dir}/ImGui/backends/imgui_impl_opengl3.cpp"

	}

	includedirs
	{
		"src",
		"%{wks.location}/Engine/src",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuizmo}",
		"%{IncludeDir.nfd}",
		"%{IncludeDir.AudioToWav}",
		"%{IncludeDir.IconFontCppHeaders}"
	}

	links
	{
		"ImGui",
		"nfd",
		"AudioToWav",
		"glfw",
		"Engine"
	}

	defines
	{
		"__EDITOR__",
		"GLFW_DLL",
		"ENGINE_LINK_DLL",
		"IMGUI_DEFINE_MATH_OPERATORS"
	}

	filter "files:vendor/ImGuizmo/**.cpp"
		flags { "NoPCH" }

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