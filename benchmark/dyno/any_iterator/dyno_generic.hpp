// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef BENCHMARK_ANY_ITERATOR_DYNO_GENERIC_HPP
#define BENCHMARK_ANY_ITERATOR_DYNO_GENERIC_HPP

#include "caramel-poly/Poly.hpp"


namespace dyno_generic {

constexpr auto increment_LABEL = POLY_FUNCTION_LABEL("increment");
constexpr auto decrement_LABEL = POLY_FUNCTION_LABEL("decrement");
constexpr auto dereference_LABEL = POLY_FUNCTION_LABEL("dereference");

template <typename Reference>
struct Iterator : decltype(caramel_poly::requires(
	caramel_poly::MoveConstructible{},
	increment_LABEL = caramel_poly::function<void (caramel_poly::SelfPlaceholder&)>,
	dereference_LABEL = caramel_poly::function<Reference (caramel_poly::SelfPlaceholder&)>,
	caramel_poly::EQUAL_LABEL = caramel_poly::function<bool (caramel_poly::SelfPlaceholder const&, caramel_poly::SelfPlaceholder const&)>
)) { };

template <typename Value, typename StoragePolicy, typename VTablePolicy, typename Reference = Value&>
struct any_iterator {
	using value_type = Value;
	using reference = Reference;

	template <typename It>
	explicit any_iterator(It it)
		: poly_{std::move(it), caramel_poly::makeConceptMap(
			increment_LABEL = [](It& self) { ++self; },
			dereference_LABEL = [](It& self) -> decltype(auto) { return *self; },
			caramel_poly::EQUAL_LABEL = [](It const& a, It const& b) -> bool { return a == b; }
		)}
	{ }

	any_iterator(any_iterator&& other)
		: poly_{std::move(other.poly_)}
	{ }

	any_iterator& operator++() {
		poly_.virtual_(increment_LABEL)(poly_);
		return *this;
	}

	reference operator*() {
		return poly_.virtual_(dereference_LABEL)(poly_);
	}

	friend bool operator==(any_iterator const& a, any_iterator const& b) {
		return a.poly_.virtual_(caramel_poly::EQUAL_LABEL)(a.poly_, b.poly_);
	}

private:
	caramel_poly::Poly<Iterator<reference>, StoragePolicy, VTablePolicy> poly_;
};

using remote_storage = dyno_generic::any_iterator<
	int, caramel_poly::RemoteStorage<>, caramel_poly::VTable<caramel_poly::Remote<caramel_poly::Everything>>
>;

using local_storage = dyno_generic::any_iterator<
	int, caramel_poly::LocalStorage<sizeof(std::vector<int>::iterator)>, caramel_poly::VTable<caramel_poly::Remote<caramel_poly::Everything>>
>;

using local_storage_inlined_vtable = dyno_generic::any_iterator<
	int,
	caramel_poly::LocalStorage<sizeof(std::vector<int>::iterator)>,
	caramel_poly::VTable<
		caramel_poly::Local<
			caramel_poly::Only<decltype(increment_LABEL),
			decltype(dereference_LABEL),
			decltype(caramel_poly::EQUAL_LABEL)
			>
		>,
		caramel_poly::Remote<caramel_poly::EverythingElse>
		>
	>;
} // end namespace dyno_generic

#endif // BENCHMARK_ANY_ITERATOR_DYNO_GENERIC_HPP
