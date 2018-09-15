// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#ifndef ANY_ITERATOR_HPP
#define ANY_ITERATOR_HPP

#include <caramel-poly/Poly.hpp>

#include <cassert>
#include <iterator>
#include <type_traits>
#include <utility>
#include <array>

constexpr auto increment_LABEL = POLY_FUNCTION_LABEL("increment");
constexpr auto decrement_LABEL = POLY_FUNCTION_LABEL("decrement");
constexpr auto dereference_LABEL = POLY_FUNCTION_LABEL("dereference");
constexpr auto advance_LABEL = POLY_FUNCTION_LABEL("advance");
constexpr auto distance_LABEL = POLY_FUNCTION_LABEL("distance");

// This is the definition of an Iterator concept using a "generic" language.
// Instead of defining specific methods that must be defined, it defines its
// interface in terms of compile-time strings, assuming these may be fulfilled
// in possibly many different ways.
template <typename Reference>
struct Iterator : decltype(caramel_poly::requires(
	caramel_poly::CopyConstructible{},
	caramel_poly::CopyAssignable{},
	caramel_poly::Destructible{},
	caramel_poly::Swappable{},
	increment_LABEL = caramel_poly::function<void (caramel_poly::SelfPlaceholder&)>,
	dereference_LABEL = caramel_poly::function<Reference (caramel_poly::SelfPlaceholder&)>
)) { };

template <typename Reference>
struct InputIterator : decltype(caramel_poly::requires(
	Iterator<Reference>{},
	caramel_poly::EqualityComparable{}
)) { };

template <typename Reference>
struct ForwardIterator : decltype(caramel_poly::requires(
	InputIterator<Reference>{},
	caramel_poly::DefaultConstructible{}
)) { };

template <typename Reference>
struct BidirectionalIterator : decltype(caramel_poly::requires(
	ForwardIterator<Reference>{},
	decrement_LABEL = caramel_poly::function<void (caramel_poly::SelfPlaceholder&)>
)) { };

template <typename Reference, typename Difference>
struct RandomAccessIterator : decltype(caramel_poly::requires(
	BidirectionalIterator<Reference>{},
	advance_LABEL = caramel_poly::function<void (caramel_poly::SelfPlaceholder&, Difference)>,
	distance_LABEL = caramel_poly::function<Difference (caramel_poly::SelfPlaceholder const&, caramel_poly::SelfPlaceholder const&)>
)) { };


// This is some kind of concept map; it maps the "generic" iterator interface
// (method names as compile-time strings) to actual implementations for a
// specific iterator type.
template <typename Ref, typename T>
auto const caramel_poly::defaultConceptMap<Iterator<Ref>, T> = caramel_poly::makeConceptMap(
	increment_LABEL = [](T& self) { ++self; },
	dereference_LABEL = [](T& self) -> Ref { return *self; }
);

template <typename Ref, typename T>
auto const caramel_poly::defaultConceptMap<BidirectionalIterator<Ref>, T> = caramel_poly::makeConceptMap(
	decrement_LABEL = [](T& self) -> void { --self; }
);

template <typename Ref, typename Diff, typename T>
auto const caramel_poly::defaultConceptMap<RandomAccessIterator<Ref, Diff>, T> = caramel_poly::makeConceptMap(
	advance_LABEL = [](T& self, Diff diff) -> void {
			std::advance(self, diff);
		},
	distance_LABEL = [](T const& first, T const& last) -> Diff {
			return std::distance(first, last);
		}
	);

namespace detail {
template <typename Category, typename Reference, typename Difference>
struct iterator_category_to_concept;

template <typename Reference, typename Difference>
struct iterator_category_to_concept<std::input_iterator_tag, Reference, Difference> {
	using type = InputIterator<Reference>;
};

template <typename Reference, typename Difference>
struct iterator_category_to_concept<std::forward_iterator_tag, Reference, Difference> {
	using type = ForwardIterator<Reference>;
};

template <typename Reference, typename Difference>
struct iterator_category_to_concept<std::bidirectional_iterator_tag, Reference, Difference> {
	using type = BidirectionalIterator<Reference>;
};

template <typename Reference, typename Difference>
struct iterator_category_to_concept<std::random_access_iterator_tag, Reference, Difference> {
	using type = RandomAccessIterator<Reference, Difference>;
};
} // end namespace detail

  // This defines a type-erased wrapper satisfying a specific concept (Iterator)
  // and providing the given interface (methods, etc..). The choice of how to
  // store the type-erased object and how to implement the VTable should be done
  // here, and should be disjoint from the actual concept definition and concept
  // map above.
template <
	typename Value,
	typename Category,
	typename Reference = Value&,
	typename Difference = std::ptrdiff_t
>
struct any_iterator {
	using iterator_category = Category;
	using value_type = Value;
	using reference = Reference;
	using pointer = value_type*;
	using difference_type = Difference;

private:
	using Concept = typename detail::iterator_category_to_concept<
		iterator_category, reference, difference_type
	>::type;
	using ActualConcept = decltype(caramel_poly::requires(
		Concept{},
		caramel_poly::TypeId{} // For assertion in operator==
	));

	using Storage = caramel_poly::LocalStorage<
		sizeof(std::array<int, 4>::iterator),
		alignof(std::array<int, 4>::iterator)
		>;
	caramel_poly::Poly<ActualConcept, Storage> poly_;

public:
	template <typename It>
	explicit any_iterator(It it)
		: poly_{std::move(it)}
	{
		using IteratorTraits = std::iterator_traits<It>;
		using Source_value_type = typename IteratorTraits::value_type;
		using Source_reference = typename IteratorTraits::reference;
		using Source_category = typename IteratorTraits::iterator_category;
		using Source_difference_type = typename IteratorTraits::difference_type;

		static_assert(std::is_convertible<Source_value_type, value_type>{},
			"The 'value_type' of the erased iterator must be convertible to that of "
			"the 'any_iterator'");

		static_assert(std::is_convertible<Source_reference, reference>{},
			"The 'reference' of the erased iterator must be convertible to that of "
			"the 'any_iterator'");

		static_assert(std::is_base_of<iterator_category, Source_category>{},
			"The 'iterator_category' of the erased iterator must be at least as "
			"powerful as that of the 'any_iterator'");

		static_assert(std::is_same<difference_type, Source_difference_type>{},
			"The 'difference_type' of the erased iterator must match that of the "
			"'any_iterator'");
	}

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

	template <bool True = true, typename = std::enable_if_t<True &&
		std::is_base_of<std::bidirectional_iterator_tag, iterator_category>{}
	>> any_iterator& operator--() {
		poly_.virtual_(decrement_LABEL)(poly_);
		return *this;
	}

	reference operator*() {
		return poly_.virtual_(dereference_LABEL)(poly_);
	}

	friend bool operator==(any_iterator const& a, any_iterator const& b) {
		// #TODO_Caramel
		//assert(a.poly_.virtual_(caramel_poly::TYPEID_LABEL)() == b.poly_.virtual_(caramel_poly::TYPEID_LABEL)());
		return a.poly_.virtual_(caramel_poly::EQUAL_LABEL)(a.poly_, b.poly_);
	}

	friend bool operator!=(any_iterator const& a, any_iterator const& b) {
		return !(a == b);
	}
};

#endif // ANY_ITERATOR_HPP
