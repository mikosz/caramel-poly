function googlebenchmark_include()
	includedirs("external/benchmark/include")
end

function add_googlebenchmark_libdir()
	local benchmark_dir = path.join(_MAIN_SCRIPT_DIR, "external/benchmark/src/")
	
	filter { "action:vs*", "configurations:Debug*" }
		libdirs(path.join(benchmark_dir, "Debug/"))
	filter { "action:vs*", "configurations:Release*" }
		libdirs(path.join(benchmark_dir, "Release/"))
	filter { "action:gmake" }
		libdirs(path.join(benchmark_dir, "src/"))
	filter {}
end

function link_googlebenchmark_lib()
	links "benchmark"
	filter "platforms:Win*"
		links "Shlwapi"
	filter {}
end

function use_googlebenchmark()
	googlebenchmark_include()
	add_googlebenchmark_libdir()
	link_googlebenchmark_lib()
end
