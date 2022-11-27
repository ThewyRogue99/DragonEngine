workspace "GameAssembly"
	architecture "x64"
	startproject "ScriptAssembly"
	
	configurations
	{
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

project "ScriptAssembly"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir ("%{wks.location}/Binary/Scripts")
	objdir ("%{wks.location}/Binary/Scripts/Intermediates")

	files
	{
		"Source/**.cs"
	}

	links
	{
		"%{wks.location}/Binary/Scripts/DE_ScriptCore.dll"
	}

	filter "configurations:Release"
		optimize "On"
		symbols "Default"