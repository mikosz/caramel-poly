// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include "model.hpp"

#include "caramel-poly/Poly.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <type_traits>
#include <utility>


// This benchmark measures the overhead of dispatching methods through a
// type-erased wrapper with different storage policies.

template <typename StoragePolicy, typename T>
static void BM_dispatch_single(benchmark::State& state) {
	model<StoragePolicy> m{T{}};
	benchmark::DoNotOptimize(m);
	while (state.KeepRunning()) {
		m.f1();
		m.f2();
		m.f3();
	}
}

template <std::size_t Bytes>
using WithSize = std::aligned_storage_t<Bytes>;

BENCHMARK_TEMPLATE(BM_dispatch_single, inheritance_tag,         WithSize<8>);
BENCHMARK_TEMPLATE(BM_dispatch_single, caramel::poly::RemoteStorage<>,  WithSize<8>);
BENCHMARK_TEMPLATE(BM_dispatch_single, caramel::poly::SBOStorage<4>,    WithSize<8>);
BENCHMARK_TEMPLATE(BM_dispatch_single, caramel::poly::SBOStorage<8>,    WithSize<8>);
BENCHMARK_TEMPLATE(BM_dispatch_single, caramel::poly::SBOStorage<16>,   WithSize<8>);
BENCHMARK_TEMPLATE(BM_dispatch_single, caramel::poly::SBOStorage<32>,   WithSize<8>);
BENCHMARK_TEMPLATE(BM_dispatch_single, caramel::poly::LocalStorage<32>, WithSize<8>);
