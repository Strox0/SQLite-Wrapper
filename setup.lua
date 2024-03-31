outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

workspace "SQLite-Wrapper"
	architecture "x64"
	configurations {"Debug","Release"}

    filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }
      systemversion "latest"

    filter "configurations:Debug"
      defines { "_DEBUG" }
      runtime "Debug"
      symbols "On"

    filter "configurations:Release"
      defines { "NDEBUG" }
      runtime "Release"
      optimize "On"
      symbols "Off"

project "SQLite-Wrapper"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("SQLite-Wrapper/bin/" .. outputdir .. "/")
	objdir ("SQLite-Wrapper/bin-int/" .. outputdir .. "/")

	files {
		"src/**.cpp",
		"src/**.h"
	}

	includedirs {
		"src/sqlite/",
		"src/"
	}