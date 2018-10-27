structure = require "structure"

include "premake5.cfg.lua"
include "googletest.lua"
include "googlebenchmark.lua"

ROOT_DIR = path.getabsolute(".")

location "build"	

workspace "caramel-poly"

	configurations { "DebugStatic", "DebugMemcheckStatic", "ProfileBuildTimes", "ReleaseStatic" }
	
	platforms { "Win64", "Linux" }
	
	language "C++"

	-- Configuration specific settings
	
	filter "configurations:Debug*"
		symbols "On"
		
	filter "configurations:DebugMemcheckStatic"
		buildoptions { "/Ob0" } -- disable inline expansion
		flags { "NoFramePointer" }
	
	filter "configurations:ProfileBuildTimes"
		buildoptions { "/Bt+ /showIncludes /nologo- /FC" }
	
	filter "configurations:Release*"
		defines { "NDEBUG" }
		optimize "On"
	
	-- Platform specific settings
	
	filter "platforms:Win64"
		system "Windows"
		architecture "x64"
		
	filter "platforms:Linux"
		system "Linux"
		architecture "x64"
		
	filter {}

	filter "action:gmake"
		linkoptions { "-pthread" }
	filter {}
	
	-- C++ Standard

	filter "action:vs*"
		buildoptions { "/std:c++latest" }
		defines { "GTEST_LANG_CXX11=1" }
	filter {}

	filter "action:gmake"
		buildoptions { "-std=c++17" }
		defines { "GTEST_LANG_CXX11=1" }
	filter {}
		
	-- Warnings
	
	warnings "Extra"
	flags { "FatalWarnings" }
	
	flags { "MultiProcessorCompile" }
	
	filter "action:vs*"
		defines { "_SCL_SECURE_NO_WARNINGS" }
	filter {}

	structure.header_project("caramel-poly", "src")
	
	structure.executable_project("caramel-poly-test", "test", false, function()
			use_googletest()
		end
		)

	structure.executable_project("caramel-poly-benchmark", "benchmark", false, function()
			use_googlebenchmark()
		end
		)

	structure.create_build_all_project()
	structure.create_install_project()
	structure.create_run_tests_project()
