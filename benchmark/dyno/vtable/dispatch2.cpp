// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include "model.hpp"

#include "caramel-poly/Poly.hpp"

#include <benchmark/benchmark.h>

// This benchmark measures the overhead of dispatching virtual calls using
// different vtable policies, and inheritance.

template <typename VTablePolicy>
static void BM_dispatch2(benchmark::State& state) {
	unsigned int x = 0;
	model<VTablePolicy> m{x};
	int const N = state.range(0);
	while (state.KeepRunning()) {
		for (int i = 0; i != N; ++i) {
			benchmark::DoNotOptimize(m);
			m.f1();
			m.f2();
		}
	}
}

template <typename ...InlineMethods>
using inline_only = caramel_poly::VTable<
	caramel_poly::Local<caramel_poly::Only<InlineMethods...>>,
	caramel_poly::Remote<caramel_poly::EverythingElse>
>;

static constexpr int D2_N = 100;
BENCHMARK_TEMPLATE(BM_dispatch2, inheritance_tag)->Arg(D2_N);
BENCHMARK_TEMPLATE(BM_dispatch2, inline_only<>)->Arg(D2_N);
BENCHMARK_TEMPLATE(BM_dispatch2, inline_only<decltype(f1_LABEL)>)->Arg(D2_N);
BENCHMARK_TEMPLATE(BM_dispatch2, inline_only<decltype(f1_LABEL), decltype(f2_LABEL)>)->Arg(D2_N);
