// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include "model.hpp"

#include "caramel-poly/Poly.hpp"

#include <benchmark/benchmark.h>

#include <string>


// This benchmark measures the overhead of performing a `swap()` operation
// with different storage policies, when the two PolymorphicStorages hold
// objects of the same type.

template <typename StoragePolicy>
static void BM_swap_same(benchmark::State& state) {
	model<StoragePolicy> a{std::string{"barbaz"}};
	model<StoragePolicy> b{std::string{"foobar"}};
	benchmark::DoNotOptimize(a);
	benchmark::DoNotOptimize(b);

	while (state.KeepRunning()) {
		a.swap(b);
		b.swap(a);
	}
}

BENCHMARK_TEMPLATE(BM_swap_same, inheritance_tag);
BENCHMARK_TEMPLATE(BM_swap_same, caramel_poly::SBOStorage<4>);
BENCHMARK_TEMPLATE(BM_swap_same, caramel_poly::SBOStorage<8>);
BENCHMARK_TEMPLATE(BM_swap_same, caramel_poly::SBOStorage<16>);
BENCHMARK_TEMPLATE(BM_swap_same, caramel_poly::SBOStorage<sizeof(std::string)>);
// #TODO_Caramel: re-enable if adding FallbackStorage
//BENCHMARK_TEMPLATE(BM_swap_same, caramel_poly::FallbackStorage<caramel_poly::local_storage<8>, caramel_poly::remote_storage>);
BENCHMARK_TEMPLATE(BM_swap_same, caramel_poly::RemoteStorage<>);
BENCHMARK_TEMPLATE(BM_swap_same, caramel_poly::LocalStorage<sizeof(std::string)>);
