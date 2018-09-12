// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include "model.hpp"

#include "caramel-poly/Poly.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <type_traits>
#include <utility>


// This benchmark measures the overhead of constructing type-erased wrappers
// from a concrete model, with different storage policies.

template <typename StoragePolicy, typename T>
static void BM_ctor(benchmark::State& state) {
	T x{};
	while (state.KeepRunning()) {
		benchmark::DoNotOptimize(x);
		model<StoragePolicy> m{x};
		benchmark::DoNotOptimize(m);
	}
}

template <std::size_t Bytes>
using WithSize = std::aligned_storage_t<Bytes>;

BENCHMARK_TEMPLATE(BM_ctor, inheritance_tag,         WithSize<4>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::RemoteStorage,    WithSize<4>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::SBOStorage<4>,    WithSize<4>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::SBOStorage<8>,    WithSize<4>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::SBOStorage<16>,   WithSize<4>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::LocalStorage<16>, WithSize<4>);

BENCHMARK_TEMPLATE(BM_ctor, inheritance_tag,         WithSize<16>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::RemoteStorage,    WithSize<16>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::SBOStorage<4>,    WithSize<16>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::SBOStorage<8>,    WithSize<16>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::SBOStorage<16>,   WithSize<16>);
BENCHMARK_TEMPLATE(BM_ctor, caramel_poly::LocalStorage<16>, WithSize<16>);
