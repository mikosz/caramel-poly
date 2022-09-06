// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Poly.hpp"

#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

namespace /* anonymous */ {

constexpr auto increment_LABEL = POLY_FUNCTION_LABEL("increment");
constexpr auto decrement_LABEL = POLY_FUNCTION_LABEL("decrement");
constexpr auto dereference_LABEL = POLY_FUNCTION_LABEL("dereference");
constexpr auto advance_LABEL = POLY_FUNCTION_LABEL("advance");
constexpr auto distance_LABEL = POLY_FUNCTION_LABEL("distance");

// This example shows how vtable traits can be used to customize the way
// dynamic dispatch is done, and in particular how one can control which
// function pointers are stored where. In this specific example, the goal
// is to store functions that are called often in the object itself, but
// other functions (e.g. the destructor) in a remote vtable.

template <typename Reference>
struct Iterator : decltype(caramel::poly::require(
	caramel::poly::DefaultConstructible{},
	caramel::poly::CopyConstructible{},
	caramel::poly::CopyAssignable{},
	caramel::poly::Destructible{},
	caramel::poly::Swappable{},
	caramel::poly::EqualityComparable{},
	increment_LABEL = caramel::poly::function<void(caramel::poly::SelfPlaceholder&)>,
	decrement_LABEL = caramel::poly::function<void(caramel::poly::SelfPlaceholder&)>,
	dereference_LABEL = caramel::poly::function<Reference(caramel::poly::SelfPlaceholder&)>,
	advance_LABEL = caramel::poly::function<void(caramel::poly::SelfPlaceholder&, std::ptrdiff_t)>,
	distance_LABEL = caramel::poly::function<std::ptrdiff_t(caramel::poly::SelfPlaceholder const&, caramel::poly::SelfPlaceholder const&)>
)) { };

} // anonymous namespace

template <typename Ref, typename T>
auto const caramel::poly::defaultTraitMap<Iterator<Ref>, T> = caramel::poly::makeTraitMap(
	increment_LABEL = [](T& self) { ++self; },
	decrement_LABEL = [](T& self) -> void { --self; },
	dereference_LABEL = [](T& self) -> Ref { return *self; },
	advance_LABEL = [](T& self, std::ptrdiff_t diff) -> void {
			std::advance(self, diff);
		},
	distance_LABEL = [](T const& first, T const& last) -> std::ptrdiff_t {
			return std::distance(first, last);
		}
);

namespace /* anonymous */ {

template <typename Value, typename Reference = Value & >
struct any_iterator {
	using iterator_category = std::random_access_iterator_tag;
	using value_type = Value;
	using reference = Reference;
	using pointer = value_type * ;
	using difference_type = std::ptrdiff_t;

private:
	using Trait = Iterator<reference>;
	using Storage = caramel::poly::RemoteStorage<>;
	using VTable = caramel::poly::VTable<
		caramel::poly::Local<
			caramel::poly::Only<
				decltype(increment_LABEL),
				decltype(caramel::poly::EQUAL_LABEL),
				decltype(dereference_LABEL)
				>
			>,
			caramel::poly::Remote<caramel::poly::EverythingElse>
		>;
	caramel::poly::Poly<Trait, Storage, VTable> poly_;

public:
	template <typename It>
	explicit any_iterator(It it)
		: poly_{ std::move(it) }
	{ }

	any_iterator(any_iterator const& other) = default;
	any_iterator(any_iterator&& other) = default;
	any_iterator& operator=(any_iterator const& other) = default;
	any_iterator& operator=(any_iterator&& other) = default;
	~any_iterator() = default;

	void swap(any_iterator& other) {
		using std::swap;
		swap(this->poly_, other.poly_);
	}

	friend void swap(any_iterator& a, any_iterator& b) { a.swap(b); }

	any_iterator& operator++() {
		poly_.virtual_(increment_LABEL)(poly_);
		return *this;
	}

	any_iterator& operator--() {
		poly_.virtual_(decrement_LABEL)(poly_);
		return *this;
	}

	reference operator*() {
		return poly_.virtual_(dereference_LABEL)(poly_);
	}

	friend bool operator==(any_iterator const& a, any_iterator const& b) {
		return a.poly_.virtual_(caramel::poly::EQUAL_LABEL)(a.poly_, b.poly_);
	}

	friend bool operator!=(any_iterator const& a, any_iterator const& b) {
		return !(a == b);
	}
};

TEST(DynoTest, VTableTraitsExample) {
	using Iterator = any_iterator<int>;
	std::vector<int> input = { 1, 2, 3, 4 };
	std::vector<int> result;
	Iterator first{ input.begin() }, last{ input.end() };

	for (; first != last; ++first) {
		result.push_back(*first);
	}
	EXPECT_EQ(result, input);
}

} // anonymous namespace
