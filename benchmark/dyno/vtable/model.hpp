// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef BENCHMARK_VTABLE_MODEL_HPP
#define BENCHMARK_VTABLE_MODEL_HPP

#include "caramel-poly/Poly.hpp"
#include <benchmark/benchmark.h>
#include <memory>
#include <utility>

constexpr auto f1_LABEL = METHOD_NAME("f1");
constexpr auto f2_LABEL = METHOD_NAME("f2");
constexpr auto f3_LABEL = METHOD_NAME("f3");
constexpr auto f4_LABEL = METHOD_NAME("f4");

struct Concept : decltype(caramel_poly::requires(
	caramel_poly::CopyConstructible{},
	caramel_poly::Swappable{},
	caramel_poly::Destructible{},
	caramel_poly::Storable{},
	f1_LABEL = caramel_poly::function<void(caramel_poly::SelfPlaceholder&)>,
	f2_LABEL = caramel_poly::function<void(caramel_poly::SelfPlaceholder&)>,
	f3_LABEL = caramel_poly::function<void(caramel_poly::SelfPlaceholder&)>,
	f4_LABEL = caramel_poly::function<void(caramel_poly::SelfPlaceholder&)>
)) { };

template <typename T>
auto const caramel_poly::defaultConceptMap<Concept, T> = caramel_poly::makeConceptMap(
	f1_LABEL = [](T& self) { ++self; benchmark::DoNotOptimize(self); },
	f2_LABEL = [](T& self) { ++self; benchmark::DoNotOptimize(self); },
	f3_LABEL = [](T& self) { ++self; benchmark::DoNotOptimize(self); },
	f4_LABEL = [](T& self) { ++self; benchmark::DoNotOptimize(self); }
);

template <typename VTablePolicy>
struct model {
	template <typename T>
	explicit model(T t)
		: poly_{std::move(t)}
	{ }

	void f1() { poly_.virtual_(f1_LABEL)(poly_); }
	void f2() { poly_.virtual_(f2_LABEL)(poly_); }
	void f3() { poly_.virtual_(f3_LABEL)(poly_); }
	void f4() { poly_.virtual_(f4_LABEL)(poly_); }

private:
	caramel_poly::Poly<Concept, caramel_poly::LocalStorage<8>, VTablePolicy> poly_;
};

struct inheritance_tag { };

template <>
struct model<inheritance_tag> {
	template <typename T>
	explicit model(T t)
		: self_{std::make_unique<model_t<T>>(std::move(t))}
	{ }

	void f1() { self_->f1(); }
	void f2() { self_->f2(); }
	void f3() { self_->f3(); }
	void f4() { self_->f4(); }

private:
	struct concept_t {
		virtual void f1() = 0;
		virtual void f2() = 0;
		virtual void f3() = 0;
		virtual void f4() = 0;
		virtual ~concept_t() { }
	};

	template <typename T>
	struct model_t : concept_t {
		model_t(T v) : value_{std::move(v)} { }
		void f1() override { ++value_; benchmark::DoNotOptimize(value_); }
		void f2() override { ++value_; benchmark::DoNotOptimize(value_); }
		void f3() override { ++value_; benchmark::DoNotOptimize(value_); }
		void f4() override { ++value_; benchmark::DoNotOptimize(value_); }
		T value_;
	};

	std::unique_ptr<concept_t> self_;
};

#endif // BENCHMARK_VTABLE_MODEL_HPP