// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_VTABLE_HPP__
#define CARAMELPOLY_VTABLE_HPP__

#include <type_traits>
#include <utility>

#include "detail/EraseFunction.hpp"
#include "detail/EraseSignature.hpp"
#include "detail/ConstexprMap.hpp"
#include "detail/ConstexprPair.hpp"
#include "Concept.hpp"
#include "ConceptMap.hpp"

namespace caramel_poly {

// concept VTable
//
// A VTable is a mapping from statically-known function names to
// dynamically-known function pointers. In some sense, a vtable is
// a type-erased concept map.
//
// In addition to being `Swappable`, a type `Table` satisfying the `VTable`
// concept must provide the following functions as part of its interface:
//
// template <class ConceptMap> explicit Table(ConceptMap);
//  Semantics: Construct a vtable with the functions from a concept map.
//             Note that the actual instance of the concept map being passed
//             as a parameter is not required to be used; if that is enough
//             for the purpose of the vtable, any instance of that concept
//             map sharing the same type may be used.
//
//             Note: Concept maps have unique types, so this wording is only
//                   a way of ensuring we can statically construct a vtable
//                   from just the type of that concept map.
//
// template <class Name> constexpr auto contains(Name) const;
//  Semantics: Return whether the vtable contains a function with the given
//             name, which must be a compile-time string.
//
// template <class Name> constexpr auto operator[](Name) const;
//  Semantics: Return the function with the given name in the vtable if there
//             is one. The behavior when no such function exists in the vtable
//             is implementation defined (in most cases that's a compile-time
//             error).

//////////////////////////////////////////////////////////////////////////////
// Vtable implementations

// Class implementing a local vtable, i.e. a vtable whose storage is held
// right where the `LocalVTable` is instantiated.
template <class... Mappings>
struct LocalVTable;

template <class... Name, class... Clause>
struct LocalVTable<detail::ConstexprPair<Name, Clause>...> {
	
	template <class ConceptMap>
	constexpr explicit LocalVTable([[maybe_unused]] ConceptMap map) :
		vtbl_(detail::makeConstexprMap(
			detail::makeConstexprPair(
				Name{},
				detail::EraseFunction<typename Clause::Type>(map[Name{}])
				)...
			)
		)
	{
	}

	template <class OtherName>
	constexpr auto contains(OtherName name) const {
		return vtbl_.contains(name);
	}

	template <class OtherName>
	constexpr auto operator[](OtherName name) const {
		constexpr auto containsFunction = FunctionMap{}.contains(OtherName{});
		if constexpr (containsFunction) {
			return vtbl_[name];
		} else {
			static_assert(
				containsFunction,
				"caramel_poly::LocalVTable::operator[]: Request for a virtual function that is "
				"not in the vtable. Was this function specified in the concept that "
				"was used to instantiate this vtable? You can find the contents of the "
				"vtable and the function you were trying to access in the compiler "
				"error message, probably in the following format: "
				"`LocalVTable<CONTENTS OF VTABLE>::operator[]<FUNCTION NAME>`"
				);
		}
	}

	friend void swap(LocalVTable& lhs, LocalVTable& rhs) {
		forEach(
			keys(lhs.vtbl_),
			[&lhs, &rhs](auto key) {
					using std::swap;
					swap(lhs.vtbl_[key], rhs.vtbl_[key]);
				}
			);
	}

private:

	using FunctionMap = detail::ConstexprMap<
		detail::ConstexprPair<
			Name,
			typename detail::EraseSignature<typename Clause::Type>::Type*
			>...
		>;

	FunctionMap vtbl_;

};

#if 0

namespace detail {
template <class VTable, class ConceptMap>
static VTable const static_vtable{ConceptMap{}};
}

// Class implementing a vtable whose storage is held remotely. This is
// basically a pointer to a static instance of the specified `VTable`.
template <class VTable>
struct remote_vtable {
	template <class ConceptMap>
	constexpr explicit remote_vtable(ConceptMap)
		: vptr_{&detail::static_vtable<VTable, ConceptMap>}
	{ }

	template <class Name>
	constexpr auto operator[](Name name) const {
		return (*vptr_)[name];
	}

	template <class Name>
	constexpr auto contains(Name name) const {
		return vptr_->contains(name);
	}

	friend void swap(remote_vtable& a, remote_vtable& b) {
		using std::swap;
		swap(a.vptr_, b.vptr_);
	}

private:
	VTable const* vptr_;
};

// Class implementing a vtable that joins two other vtables.
//
// A function is first looked up in the first vtable, and in the second
// vtable if it can't be found in the first one. It is an error if a
// function is contained in both vtables, since this is most likely a
// programming error.
template <class First, class Second>
struct joined_vtable {
	template <class ConceptMap>
	constexpr explicit joined_vtable(ConceptMap map)
		: first_{map}, second_{map}
	{ }

	template <class Name>
	constexpr auto contains(Name name) const {
		return first_.contains(name) || second_.contains(name);
	}

	template <class Name>
	constexpr auto operator[](Name name) const {
		auto first_contains_function = first_.contains(name);
		auto second_contains_function = second_.contains(name);

		if constexpr (first_contains_function && second_contains_function) {
			static_assert(!first_contains_function || !second_contains_function,
				"caramel_poly::joined_vtable::operator[]: Request for a virtual function that is "
				"contained in both vtables of a joined vtable. Since this is most likely "
				"a programming error, this is not allowed. You can find the contents of "
				"the vtable and the function you were trying to access in the compiler "
				"error message, probably in the following format: "
				"`joined_vtable<VTABLE 1, VTABLE 2>::operator[]<FUNCTION NAME>`");

		} else if constexpr (!first_contains_function && !second_contains_function) {
			static_assert(first_contains_function || second_contains_function,
				"caramel_poly::joined_vtable::operator[]: Request for a virtual function that is "
				"not present in any of the joined vtables. Make sure you meant to look "
				"this function up, and otherwise check whether the two sub-vtables look "
				"as expected. You can find the contents of the joined vtables and the "
				"function you were trying to access in the compiler error message, "
				"probably in the following format: "
				"`joined_vtable<VTABLE 1, VTABLE 2>::operator[]<FUNCTION NAME>`");

		} else if constexpr (first_contains_function) {
			return first_[name];

		} else {
			return second_[name];
		}
	}

private:
	First first_;
	Second second_;
};

//////////////////////////////////////////////////////////////////////////////
// Selectors
template <class... Functions>
struct only {
	template <class All>
	constexpr auto operator()(All all) const {
		auto matched = boost::hana::make_set(Functions{}...);
		static_assert(decltype(boost::hana::is_subset(matched, all))::value,
			"caramel_poly::only: Some functions specified in this selector are not part of "
			"the concept to which the selector was applied.");
		return boost::hana::make_pair(
			boost::hana::difference(all, matched),
			matched
		);
	}
};

template <class... Functions>
struct except {
	template <class All>
	constexpr auto operator()(All all) const {
		auto not_matched = boost::hana::make_set(Functions{}...);
		static_assert(decltype(boost::hana::is_subset(not_matched, all))::value,
			"caramel_poly::except: Some functions specified in this selector are not part of "
			"the concept to which the selector was applied.");
		return boost::hana::make_pair(
			not_matched,
			boost::hana::difference(all, not_matched)
		);
	}
};

struct everything {
	template <class All>
	constexpr auto operator()(All all) const {
		return boost::hana::make_pair(boost::hana::make_set(), all);
	}
};

using everything_else = everything;

namespace detail {
template <class T>
struct is_valid_selector : boost::hana::false_ { };

template <class... Methods>
struct is_valid_selector<caramel_poly::only<Methods...>>
	: boost::hana::true_
{ };

template <class... Methods>
struct is_valid_selector<caramel_poly::except<Methods...>>
	: boost::hana::true_
{ };

template <>
struct is_valid_selector<caramel_poly::everything>
	: boost::hana::true_
{ };
} // end namespace detail

  //////////////////////////////////////////////////////////////////////////////
  // Vtable policies
template <class Selector>
struct local {
	static_assert(detail::is_valid_selector<Selector>::value,
		"caramel_poly::local: Provided invalid selector. Valid selectors are "
		"'caramel_poly::only<METHODS...>', 'caramel_poly::except<METHODS...>', "
		"'caramel_poly::everything', and 'caramel_poly::everything_else'.");

	template <class Concept, class Functions>
	static constexpr auto create(Concept, Functions functions) {
		return boost::hana::unpack(functions, [](auto ...f) {
			using VTable = caramel_poly::LocalVTable<
				boost::hana::pair<decltype(f), decltype(Concept{}.get_signature(f))>...
			>;
			return boost::hana::basic_type<VTable>{};
		});
	}

	Selector selector;
};

template <class Selector>
struct remote {
	static_assert(detail::is_valid_selector<Selector>::value,
		"caramel_poly::remote: Provided invalid selector. Valid selectors are "
		"'caramel_poly::only<METHODS...>', 'caramel_poly::except<METHODS...>', "
		"'caramel_poly::everything', and 'caramel_poly::everything_else'.");

	template <class Concept, class Functions>
	static constexpr auto create(Concept, Functions functions) {
		return boost::hana::template_<caramel_poly::remote_vtable>(
			caramel_poly::local<Selector>::create(Concept{}, functions)
			);
	}

	Selector selector;
};

namespace detail {
// Returns whether a vtable is empty, such that we can completely skip it
// when composing policies below.
template <class VTable>
struct is_empty_vtable : boost::hana::false_ { };

template <>
struct is_empty_vtable<caramel_poly::LocalVTable<>> : boost::hana::true_ { };
} // end namespace detail

template <class Concept, class Policies>
constexpr auto generate_vtable(Policies policies) {
	auto functions = boost::hana::to_set(caramel_poly::clause_names(Concept{}));
	auto state = boost::hana::make_pair(functions, boost::hana::basic_type<caramel_poly::LocalVTable<>>{});
	auto result = boost::hana::fold_left(policies, state, [](auto state, auto policy) {
		auto functions = boost::hana::first(state);
		auto vtable = boost::hana::second(state);

		auto selector_split = policy.selector(functions);
		auto remaining = boost::hana::first(selector_split);
		auto matched = boost::hana::second(selector_split);

		if constexpr (detail::is_empty_vtable<class decltype(vtable)::type>{}) {
			auto new_vtable = decltype(policy.create(Concept{}, matched)){};
			return boost::hana::make_pair(remaining, new_vtable);
		} else {
			auto new_vtable = boost::hana::basic_type<
				caramel_poly::joined_vtable<
				class decltype(vtable)::type,
				class decltype(policy.create(Concept{}, matched))::type
				>
			>{};
			return boost::hana::make_pair(remaining, new_vtable);
		}
	});

	constexpr bool all_functions_were_taken = decltype(boost::hana::length(boost::hana::first(result)))::value == 0;
	static_assert(all_functions_were_taken,
		"caramel_poly::vtable: The policies specified in the vtable did not fully cover all "
		"the functions provided by the concept. Some functions were not mapped to "
		"any vtable, which is an error");
	return boost::hana::second(result);
}

// Policy-based interface for defining vtables.
//
// This type does not model the `VTable` concept itself; instead, it is used
// to generate a type that models that concept.
//
// A `vtable` is parameterized on one or more policies, which specify how
// the vtable is implemented under the hood. Some policies can be further
// parameterized using a `Selector`, in which case the functions specified
// by the `Selector` are the ones to which the policy applies. Policies
// provided by the library are:
//
//  caramel_poly::remote<Selector>
//    All functions selected by `Selector` will be stored in a remote vtable.
//    The vtable object is just a pointer to an actual vtable, and each access
//    to the vtable requires one indirection. In vanilla C++, this is the usual
//    vtable implementation.
//
//  caramel_poly::local<Selector>
//    All functions selected by `Selector` will be stored in a local vtable.
//    The vtable object will actually contain function pointers for all the
//    selected functions. When accessing a virtual function, no additional
//    indirection is required (compared to a vtable stored remotely), at the
//    cost of space inside the vtable object.
//
//
// A selector is a type that selects a subset of functions defined by a concept.
// Selectors are used to pick which policy applies to which functions when
// defining a `vtable`. For example, one might want to define a vtable where
// all the functions except one (say `"f"`) are stored remotely, with `"f"`
// being stored locally. This can be achieved by using the `caramel_poly::remote` policy
// with a selector that picks all functions except `"f"`, and the `caramel_poly::local`
// policy with a selector that picks everything (all that remains). Note that
// when multiple selectors are specified, functions picked by earlier selectors
// will effectively be removed from the concept for later selectors, which
// supports this use case. Otherwise, one would have to specify that the
// `caramel_poly::local` contains everything except the `"f"` function, which is
// cumbersome. Selectors provided by the library are:
//
//  caramel_poly::only<functions...>
//    Picks only the specified functions from a concept. `functions` must be
//    compile-time strings, such as `caramel_poly::only<decltype("foo"_s), decltype("bar"_s)>`.
//
//  caramel_poly::except<functions...>
//    Picks all but the specified functions from a concept. `functions` must
//    be compile-time strings, such as `caramel_poly::except<decltype("foo"_s), decltype("bar"_s)>`.
//
//  caramel_poly::everything
//    Picks all the functions from a concept.
//
//  caramel_poly::everything_else
//    Equivalent to `caramel_poly::everything`, but prettier to read when other
//    policies are used before it.
template <class... Policies>
struct vtable {
	template <class Concept>
	using apply = class decltype(
		caramel_poly::generate_vtable<Concept>(boost::hana::basic_tuple<Policies...>{})
		)::type;
};

#endif

} // namespace caramel_poly

#endif /* CARAMELPOLY_VTABLE_HPP__ */
