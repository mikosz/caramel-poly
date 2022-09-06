// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_CONCEPTMAP_HPP__
#define CARAMELPOLY_CONCEPTMAP_HPP__

#include <type_traits>
#include <utility>

#include "detail/DefaultConstructibleLambda.hpp"
#include "detail/ConstexprPair.hpp"
#include "detail/ConstexprMap.hpp"
#include "detail/BindSignature.hpp"
#include "Trait.hpp"
#include "dsl.hpp"

namespace caramel::poly {

// A trait map is a statically-known mapping from functions implemented by
// a type `T` to functions defined by a trait. A trait map is what's being
// used to fill the vtable extracted from a trait definition. An instance of
// this type is never created as-is; `caramel::poly::makeTraitMap` must always be used.
//
// Note that everything in the trait map is known statically. Specifically,
// the types of the functions in the trait map are known statically, and
// e.g. lambdas will be stored as-is (not as function pointers).
template <class Trait, class T, class... Mappings>
struct TraitMap;

template <class Trait, class T, class... Name, class... Function>
struct TraitMap<Trait, T, detail::ConstexprPair<Name, Function>...> {
	
	constexpr TraitMap() = default;

	template <class NameArg>
	constexpr auto operator[](NameArg) const {
		constexpr bool isKnown = Functions{}.contains(NameArg{});
		if constexpr (isKnown) {
			return Functions{}[NameArg{}];
		} else {
			static_assert(
				isKnown,
				"caramel::poly::TraitMap::operator[]: Request for the implementation of a "
				"function that was not provided in the trait map. Make sure the "
				"trait map contains the proper functions, and that you're requesting "
				"the right function from the trait map. You can find the contents of "
				"the trait map and the function you were trying to access in the "
				"compiler error message, probably in the following format: "
				"`TraitMap<CONCEPT, MODEL, CONTENTS OF CONCEPT MAP>::operator[]<FUNCTION NAME>`"
				);
		}
	}

private:

	using Functions = detail::ConstexprMap<
		detail::ConstexprPair<
			Name,
			detail::DefaultConstructibleLambda<
				Function,
				typename detail::BindSignature<
					typename decltype(Trait{}.getSignature(Name{}))::Type, T
					>::Type
				>
			>...
		>;

};

// Creates a trait map associating function names to function implementations.
//
// The exact contents of the map must be pairs where the first element is a
// function name (represented as a compile-time string), and the second element
// is the implementation of that function (as a stateless function object).
//
// Note that a trait map created with this function can be incomplete. Before
// being used, it must be completed using `caramel::poly::completeTraitMap`.
template <class... Name, class... Function>
constexpr auto makeTraitMap(detail::ConstexprPair<Name, Function>... mappings) {
	auto map = detail::makeConstexprList(mappings...);
	auto keys = transform(map, [](auto e) { return e.first(); });

	static_assert(
		!hasDuplicates(decltype(keys){}),
		"caramel::poly::makeTraitMap: It looks like you have multiple entries with the "
		"same name in your trait map. This is not allowed; each entry must have "
		"a different name."
		);

	return map;
}

// Customization point for trait writers to provide default models of
// their concepts.
//
// This can be specialized by trait authors to provide a trait map that
// will be used when no custom trait map is specified. The third parameter
// can be used to define a default trait map for a family of type, by using
// `std::enable_if`.
template <class Trait, class T, class = void>
auto const defaultTraitMap = makeTraitMap();

// Customization point for users to define their models of concepts.
//
// This can be specialized by clients to provide trait maps for the concepts
// and types they wish. The third parameter can be used to define a trait
// map for a family of type, by using `std::enable_if`.
template <class Trait, class T, class = void>
auto const conceptMap = makeTraitMap();

namespace detail {

// Takes a constexpr map, and completes it by interpreting it as a trait map
// for fulfilling the given `Trait` for the given type `T`.
template <class Trait, class T, class Map>
constexpr auto completeTraitMapImpl(Map map) {
	// 1. Bring in the functions provided in the default trait map.
	auto withDefaults = mapUnion(
		makeConstexprMap(map),
		makeConstexprMap(caramel::poly::defaultTraitMap<Trait, T>)
		);

	// 2. For each refined trait, recursively complete the trait map for
	//		that Trait and merge that into the current trait map.
	auto refined = caramel::poly::detail::refinedTraits(Trait{});
	auto merged = foldLeft(withDefaults, refined, [](auto m, auto c) {
			using C = decltype(c);
			auto completed = detail::completeTraitMapImpl<C, T>(caramel::poly::conceptMap<C, T>);
			return mapUnion(m, completed);
		});

	return merged;
}

// Turns a constexpr map into a trait map.
template <class Trait, class T, class Map>
constexpr auto toTraitMap(Map) {
	return unpack(typename Map::Entries{}, [](auto... m) {
			return caramel::poly::TraitMap<Trait, T, decltype(m)...>{};
		});
}

// Returns whether a constexpr map, when interpreted as a trait map for fulfilling
// the given `Trait`, is missing any functions.
template <class Trait, class T, class Map>
constexpr auto conceptMapIsComplete = isSubset(
	caramel::poly::detail::clauseNames(Trait{}),
	keys(Map{})
	);

} // namespace detail

// Returns whether the type `T` models the given `Trait`.
//
// Specifically, checks whether it is possible to complete the trait map of
// the given type for the given trait. Usage goes as follows:
// ```
// static_assert(caramel::poly::models<Drawable, my_polygon>);
// ```
template <class Trait, class T>
constexpr auto models = detail::conceptMapIsComplete<
	Trait,
	T,
	decltype(detail::completeTraitMapImpl<Trait, T>(caramel::poly::conceptMap<Trait, T>))
	>;

namespace diagnostic {
	template <class...> struct ________________THE_CONCEPT_IS;
	template <class...> struct ________________YOUR_MODEL_IS;
	template <class...> struct ________________FUNCTIONS_MISSING_FROM_YOUR_CONCEPT_MAP;
	template <class...> struct ________________FUNCTIONS_DECLARED_IN_YOUR_CONCEPT_MAP;
	template <class...> struct ________________FUNCTIONS_REQUIRED_BY_THE_CONCEPT;
	template <class...> struct ________________EXACT_TYPE_OF_YOUR_CONCEPT_MAP;

	template <class..., bool IS_COMPLETE = false>
	constexpr void INCOMPLETE_CONCEPT_MAP() {
		static_assert(
			IS_COMPLETE,
			"caramel::poly::conceptMap: Incomplete definition of your trait map. Despite "
			"looking at the default trait map for this trait and the trait "
			"maps for all the concepts this trait refines, I can't find definitions "
			"for all the functions that the trait requires. Please make sure you did "
			"not forget to define a function in your trait map, and otherwise make "
			"sure the proper default trait maps are kicking in. You can find information "
			"to help you debug this error in the compiler error message, probably in "
			"the instantiation of the INCOMPLETE_CONCEPT_MAP<.....> function. Good luck!"
			);
	}
} // namespace diagnostic

// Turns a constexpr map into a fully cooked trait map ready for consumption
// by a vtable.
//
// The trait maps for all the concepts that `Trait` refines are merged with
// the mappings provided explicitly. For example:
// ```
// struct A : decltype(caramel::poly::require(
//	 "f"_s = caramel::poly::function<void (caramel::poly::T&)>
// )) { };
//
// struct B : decltype(caramel::poly::require(
//	 A{},
//	 "g"_s = caramel::poly::function<int (caramel::poly::T&)>
// )) { };
//
// struct Foo { };
//
// template <>
// auto const caramel::poly::conceptMap<A, Foo> = caramel::poly::makeTraitMap(
//	 "f"_s = [](Foo&) { }
// );
//
// template <>
// auto const caramel::poly::conceptMap<B, Foo> = caramel::poly::makeTraitMap(
//	 "g"_s = [](Foo&) { return 0; }
// );
//
// auto complete = caramel::poly::completeTraitMap<B, Foo>(caramel::poly::conceptMap<B, Foo>);
// // `f` is automatically pulled from `trait<A, Foo>`
// ```
//
// Furthermore, if the same function is defined in more than one trait map
// in the full refinement tree, it is undefined which one is used. Therefore,
// all of the implementations better be the same! This is easy to enforce by
// never defining a function in a trait map where the trait does not
// require that function.
//
// Despite the above, which relates to functions defined in different concepts
// that have a refinement relation, if a function is provided both in the
// default trait map and in a customized trait map for the same trait,
// the function in the customized map is preferred.
//
// Also, after looking at the whole refinement tree, including the default
// trait maps, it is an error if any function required by the trait can't
// be resolved.
template <class Trait, class T, class Map>
constexpr auto completeTraitMap(Map map) {
	auto completeMap = detail::completeTraitMapImpl<Trait, T>(map);
	auto asTraitMap = detail::toTraitMap<Trait, T>(completeMap);
	
	constexpr auto isComplete = detail::conceptMapIsComplete<Trait, T, decltype(completeMap)>;
	if constexpr (!isComplete) {
		auto required = caramel::poly::detail::clauseNames(Trait{});
		auto declared = keys(completeMap);
		auto missing = difference(required, declared);
		diagnostic::INCOMPLETE_CONCEPT_MAP<
			diagnostic::________________THE_CONCEPT_IS<Trait>,
			diagnostic::________________YOUR_MODEL_IS<T>,
			diagnostic::________________FUNCTIONS_MISSING_FROM_YOUR_CONCEPT_MAP<decltype(missing)>,
			diagnostic::________________FUNCTIONS_DECLARED_IN_YOUR_CONCEPT_MAP<decltype(declared)>,
			diagnostic::________________FUNCTIONS_REQUIRED_BY_THE_CONCEPT<decltype(required)>,
			diagnostic::________________EXACT_TYPE_OF_YOUR_CONCEPT_MAP<decltype(asTraitMap)>
		>();
	}

	return asTraitMap;
}

} // end namespace caramel::poly

#endif // CARAMELPOLY_CONCEPTMAP_HPP__
