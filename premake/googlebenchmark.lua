function googlebenchmark_include()
	includedirs("external/benchmark/include")
end

function add_googlebenchmark_libdir()
	local benchmark_dir = path.join(_MAIN_SCRIPT_DIR, "external/benchmark")
	
	filter { "platforms:Win64", "action:vs2017", "configurations:Debug*" }
		libdirs(path.join(benchmark_dir, "Win64/vs2017/Debug/"))
	filter { "platforms:Win64", "action:vs2017", "configurations:Release*" }
		libdirs(path.join(benchmark_dir, "Win64/vs2017/Release/"))
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
