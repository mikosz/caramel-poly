// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_CONCEPT_HPP__
#define CARAMELPOLY_CONCEPT_HPP__

#include <type_traits>
#include <utility>

#include "detail/CompileTimeMap.hpp"

namespace caramel_poly {

template <class... Clauses>
struct Concept;

namespace detail {

template <class Str, class Fun>
constexpr boost::hana::basic_tuple<boost::hana::pair<Str, Fun>>
expand_clauses(const boost::hana::pair<Str, Fun>&)
{ return {}; }

template <class... Clauses>
constexpr auto expand_clauses(const Concept<Clauses...>&) {
	return boost::hana::flatten(
		boost::hana::make_basic_tuple(detail::expand_clauses(Clauses{})...)
	);
}

} // namespace detail

// Returns a sequence containing all the clauses of the given Concept and
// its derived Concepts.
//
// In the returned sequence, each clause is a pair where the first element
// is the name of the clause and the second element is the clause itself
// (e.g. a `caramel_poly::function`). The order of clauses is not specified.
template <class... Clauses>
constexpr auto clauses(const Concept<Clauses...>&) {
	auto all = boost::hana::make_basic_tuple(detail::expand_clauses(Clauses{})...);
	return boost::hana::flatten(all);
}

// Returns a sequence containing the names associated to all the claused of
// the given Concept, and its derived Concepts.
//
// The order of the clause names is not specified.
template <class... Clauses>
constexpr auto clause_names(const Concept<Clauses...>& c) {
	return boost::hana::transform(caramel_poly::clauses(c), boost::hana::first);
}

// Returns a sequence of the Concepts refined by the given Concept.
//
// Only the Concepts that are refined directly by `c` are returned, i.e. we
// do not get the refined Concepts of the refined Concepts recursively.
template <class... Clauses>
constexpr auto refined_Concepts(const Concept<Clauses...>&) {
	return boost::hana::filter(boost::hana::make_basic_tuple(Clauses{}...), [](auto t) {
		constexpr bool IsBase = std::is_base_of<detail::Concept_base, decltype(t)>::value;
		return boost::hana::bool_c<IsBase>;
	});
}

namespace detail {

template <class... Clauses>
constexpr auto direct_clauses(const Concept<Clauses...>&) {
	return boost::hana::filter(boost::hana::make_basic_tuple(Clauses{}...), [](auto t) {
		constexpr bool IsBase = std::is_base_of<detail::Concept_base, decltype(t)>::value;
		return boost::hana::bool_c<!IsBase>;
	});
}

template <class... Clauses>
constexpr auto has_duplicate_clause(const Concept<Clauses...>& c) {
	auto direct = detail::direct_clauses(c);
	return detail::has_duplicates(boost::hana::transform(direct, boost::hana::first));
}

template <class... Clauses>
constexpr auto is_redefining_base_Concept_clause(const Concept<Clauses...>& c) {
	auto bases = caramel_poly::refined_Concepts(c);
	auto base_clause_names = boost::hana::unpack(bases, [](auto ...bases) {
		auto all = boost::hana::flatten(boost::hana::make_basic_tuple(caramel_poly::clauses(bases)...));
		return boost::hana::transform(all, boost::hana::first);
	});
	return boost::hana::any_of(detail::direct_clauses(c), [=](auto clause) {
		return boost::hana::contains(base_clause_names, boost::hana::first(clause));
	});
}

} // namespace detail

// A `Concept` is a collection of clauses and refined Concepts representing
// requirements for a type to model the Concept.
//
// A Concept is created by using `caramel_poly::requires`.
//
// From a `Concept`, one can generate a virtual function table by looking at
// the signatures of the functions defined in the Concept. In the future, it
// would also be possible to do much more, like getting a predicate that checks
// whether a type satisfies the Concept.
template <class... Clauses>
struct Concept {
	static_assert(!decltype(detail::has_duplicate_clause(std::declval<Concept>())){},
		"Concept: It looks like you have multiple clauses with the same "
		"name in your Concept definition. This is not allowed; each clause must "
		"have a different name.");

	static_assert(!decltype(detail::is_redefining_base_Concept_clause(std::declval<Concept>())){},
		"Concept: It looks like you are redefining a clause that is already "
		"defined in a base Concept. This is not allowed; clauses defined in a "
		"Concept must have a distinct name from clauses defined in base Concepts "
		"if there are any.");

	template <class Name>
	constexpr auto get_signature(Name name) const {
		auto clauses = boost::hana::to_map(caramel_poly::clauses(*this));
		return clauses[name];
	}
};

// Creates a `Concept` with the given clauses. Note that a clause may be a
// Concept itself, in which case the clauses of that Concept are used, and
// that, recursively. For example:
//
// ```
// template <class Reference>
// struct Iterator : decltype(caramel_poly::requires(
//   Incrementable{},
//   "dereference"_s = caramel_poly::function<Reference (caramel_poly::T&)>
//   ...
// )) { };
// ```
//
// It is recommended to make every Concept its own structure (and not just an
// alias), as above, because that ensures the uniqueness of Concepts that have
// the same clauses.
template <class... Clauses>
constexpr Concept<Clauses...> requires(Clauses...) {
	return {};
}

} // namespace caramel_poly

#endif // CARAMELPOLY_CONCEPT_HPP__
