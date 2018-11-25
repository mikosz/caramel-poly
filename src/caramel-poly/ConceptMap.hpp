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
#include "Concept.hpp"
#include "dsl.hpp"

namespace caramel::poly {

// A concept map is a statically-known mapping from functions implemented by
// a type `T` to functions defined by a concept. A concept map is what's being
// used to fill the vtable extracted from a concept definition. An instance of
// this type is never created as-is; `caramel::poly::makeConceptMap` must always be used.
//
// Note that everything in the concept map is known statically. Specifically,
// the types of the functions in the concept map are known statically, and
// e.g. lambdas will be stored as-is (not as function pointers).
template <class Concept, class T, class... Mappings>
struct ConceptMap;

template <class Concept, class T, class... Name, class... Function>
struct ConceptMap<Concept, T, detail::ConstexprPair<Name, Function>...> {
	
	constexpr ConceptMap() = default;

	template <class NameArg>
	constexpr auto operator[](NameArg) const {
		constexpr bool isKnown = Functions{}.contains(NameArg{});
		if constexpr (isKnown) {
			return Functions{}[NameArg{}];
		} else {
			static_assert(
				isKnown,
				"caramel::poly::ConceptMap::operator[]: Request for the implementation of a "
				"function that was not provided in the concept map. Make sure the "
				"concept map contains the proper functions, and that you're requesting "
				"the right function from the concept map. You can find the contents of "
				"the concept map and the function you were trying to access in the "
				"compiler error message, probably in the following format: "
				"`ConceptMap<CONCEPT, MODEL, CONTENTS OF CONCEPT MAP>::operator[]<FUNCTION NAME>`"
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
					typename decltype(Concept{}.getSignature(Name{}))::Type, T
					>::Type
				>
			>...
		>;

};

// Creates a concept map associating function names to function implementations.
//
// The exact contents of the map must be pairs where the first element is a
// function name (represented as a compile-time string), and the second element
// is the implementation of that function (as a stateless function object).
//
// Note that a concept map created with this function can be incomplete. Before
// being used, it must be completed using `caramel::poly::completeConceptMap`.
template <class... Name, class... Function>
constexpr auto makeConceptMap(detail::ConstexprPair<Name, Function>... mappings) {
	auto map = detail::makeConstexprList(mappings...);
	auto keys = transform(map, [](auto e) { return e.first(); });

	static_assert(
		!hasDuplicates(decltype(keys){}),
		"caramel::poly::makeConceptMap: It looks like you have multiple entries with the "
		"same name in your concept map. This is not allowed; each entry must have "
		"a different name."
		);

	return map;
}

// Customization point for concept writers to provide default models of
// their concepts.
//
// This can be specialized by concept authors to provide a concept map that
// will be used when no custom concept map is specified. The third parameter
// can be used to define a default concept map for a family of type, by using
// `std::enable_if`.
template <class Concept, class T, class = void>
auto const defaultConceptMap = makeConceptMap();

// Customization point for users to define their models of concepts.
//
// This can be specialized by clients to provide concept maps for the concepts
// and types they wish. The third parameter can be used to define a concept
// map for a family of type, by using `std::enable_if`.
template <class Concept, class T, class = void>
auto const conceptMap = makeConceptMap();

namespace detail {

// Takes a constexpr map, and completes it by interpreting it as a concept map
// for fulfilling the given `Concept` for the given type `T`.
template <class Concept, class T, class Map>
constexpr auto completeConceptMapImpl(Map map) {
	// 1. Bring in the functions provided in the default concept map.
	auto withDefaults = mapUnion(
		makeConstexprMap(map),
		makeConstexprMap(caramel::poly::defaultConceptMap<Concept, T>)
		);

	// 2. For each refined concept, recursively complete the concept map for
	//		that Concept and merge that into the current concept map.
	auto refined = caramel::poly::detail::refinedConcepts(Concept{});
	auto merged = foldLeft(withDefaults, refined, [](auto m, auto c) {
			using C = decltype(c);
			auto completed = detail::completeConceptMapImpl<C, T>(caramel::poly::conceptMap<C, T>);
			return mapUnion(m, completed);
		});

	return merged;
}

// Turns a constexpr map into a concept map.
template <class Concept, class T, class Map>
constexpr auto toConceptMap(Map) {
	return unpack(typename Map::Entries{}, [](auto... m) {
			return caramel::poly::ConceptMap<Concept, T, decltype(m)...>{};
		});
}

// Returns whether a constexpr map, when interpreted as a concept map for fulfilling
// the given `Concept`, is missing any functions.
template <class Concept, class T, class Map>
constexpr auto conceptMapIsComplete = isSubset(
	caramel::poly::detail::clauseNames(Concept{}),
	keys(Map{})
	);

} // namespace detail

// Returns whether the type `T` models the given `Concept`.
//
// Specifically, checks whether it is possible to complete the concept map of
// the given type for the given concept. Usage goes as follows:
// ```
// static_assert(caramel::poly::models<Drawable, my_polygon>);
// ```
template <class Concept, class T>
constexpr auto models = detail::conceptMapIsComplete<
	Concept,
	T,
	decltype(detail::completeConceptMapImpl<Concept, T>(caramel::poly::conceptMap<Concept, T>))
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
			"caramel::poly::conceptMap: Incomplete definition of your concept map. Despite "
			"looking at the default concept map for this concept and the concept "
			"maps for all the concepts this concept refines, I can't find definitions "
			"for all the functions that the concept requires. Please make sure you did "
			"not forget to define a function in your concept map, and otherwise make "
			"sure the proper default concept maps are kicking in. You can find information "
			"to help you debug this error in the compiler error message, probably in "
			"the instantiation of the INCOMPLETE_CONCEPT_MAP<.....> function. Good luck!"
			);
	}
} // namespace diagnostic

// Turns a constexpr map into a fully cooked concept map ready for consumption
// by a vtable.
//
// The concept maps for all the concepts that `Concept` refines are merged with
// the mappings provided explicitly. For example:
// ```
// struct A : decltype(caramel::poly::requires(
//	 "f"_s = caramel::poly::function<void (caramel::poly::T&)>
// )) { };
//
// struct B : decltype(caramel::poly::requires(
//	 A{},
//	 "g"_s = caramel::poly::function<int (caramel::poly::T&)>
// )) { };
//
// struct Foo { };
//
// template <>
// auto const caramel::poly::conceptMap<A, Foo> = caramel::poly::makeConceptMap(
//	 "f"_s = [](Foo&) { }
// );
//
// template <>
// auto const caramel::poly::conceptMap<B, Foo> = caramel::poly::makeConceptMap(
//	 "g"_s = [](Foo&) { return 0; }
// );
//
// auto complete = caramel::poly::completeConceptMap<B, Foo>(caramel::poly::conceptMap<B, Foo>);
// // `f` is automatically pulled from `concept<A, Foo>`
// ```
//
// Furthermore, if the same function is defined in more than one concept map
// in the full refinement tree, it is undefined which one is used. Therefore,
// all of the implementations better be the same! This is easy to enforce by
// never defining a function in a concept map where the concept does not
// require that function.
//
// Despite the above, which relates to functions defined in different concepts
// that have a refinement relation, if a function is provided both in the
// default concept map and in a customized concept map for the same concept,
// the function in the customized map is preferred.
//
// Also, after looking at the whole refinement tree, including the default
// concept maps, it is an error if any function required by the concept can't
// be resolved.
template <class Concept, class T, class Map>
constexpr auto completeConceptMap(Map map) {
	auto completeMap = detail::completeConceptMapImpl<Concept, T>(map);
	auto asConceptMap = detail::toConceptMap<Concept, T>(completeMap);
	
	constexpr auto isComplete = detail::conceptMapIsComplete<Concept, T, decltype(completeMap)>;
	if constexpr (!isComplete) {
		auto required = caramel::poly::detail::clauseNames(Concept{});
		auto declared = keys(completeMap);
		auto missing = difference(required, declared);
		diagnostic::INCOMPLETE_CONCEPT_MAP<
			diagnostic::________________THE_CONCEPT_IS<Concept>,
			diagnostic::________________YOUR_MODEL_IS<T>,
			diagnostic::________________FUNCTIONS_MISSING_FROM_YOUR_CONCEPT_MAP<decltype(missing)>,
			diagnostic::________________FUNCTIONS_DECLARED_IN_YOUR_CONCEPT_MAP<decltype(declared)>,
			diagnostic::________________FUNCTIONS_REQUIRED_BY_THE_CONCEPT<decltype(required)>,
			diagnostic::________________EXACT_TYPE_OF_YOUR_CONCEPT_MAP<decltype(asConceptMap)>
		>();
	}

	return asConceptMap;
}

} // end namespace caramel::poly

#endif // CARAMELPOLY_CONCEPTMAP_HPP__
