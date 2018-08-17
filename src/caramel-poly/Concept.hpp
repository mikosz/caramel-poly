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

#include "detail/ConstexprList.hpp"
#include "detail/ConstexprPair.hpp"
#include "dsl.hpp"

namespace caramel_poly {

template <class... Clauses>
struct Concept;

namespace detail {

struct ConceptBase {
};

template <class Name, class Signature>
constexpr auto expandClauses(const ConstexprPair<Name, Signature>&) {
	return ConstexprPair<Name, Signature>{};
}

template <typename... Clauses>
constexpr auto expandClauses(const Concept<Clauses...>&) {
	return flatten(makeConstexprList(expandClauses(Clauses{})...));
}

// Returns a sequence containing all the clauses of the given Concept and
// its derived Concepts.
//
// In the returned sequence, each clause is a pair where the first element
// is the name of the clause and the second element is the clause itself
// (e.g. a `caramel_poly::function`). The order of clauses is not specified.
template <class... Clauses>
constexpr auto clauses(const Concept<Clauses...>&) {
	return flatten(makeConstexprList(detail::expandClauses(Clauses{})...));
}

// Returns a sequence of the Concepts refined (extended) by the given Concept.
//
// Only the Concepts that are refined directly by `c` are returned, i.e. we
// do not get the refined Concepts of the refined Concepts recursively.
template <class... Clauses>
constexpr auto refinedConcepts(const Concept<Clauses...>&) {
	return filter(detail::makeConstexprList(Clauses{}...), [](auto t) {
			return std::is_base_of_v<detail::ConceptBase, decltype(t)>;
		});
}

template <class... Clauses>
constexpr auto directClauses(const Concept<Clauses...>&) {
	return filter(detail::makeConstexprList(Clauses{}...), [](auto t) {
			return !std::is_base_of_v<detail::ConceptBase, decltype(t)>;
		});
}

template <class... Clauses>
constexpr auto hasDuplicateClause(const Concept<Clauses...>& c) {
	return hasDuplicates(transform(directClauses(c), detail::first));
}

template <class... Clauses>
constexpr auto isRedefiningBaseConceptClause(const Concept<Clauses...>& c) {
	auto bases = refinedConcepts(c);
	auto baseClauses = flatten(transform(bases, [](const auto c) { return clauses(c); }));
	auto baseClauseNames = transform(baseClauses, detail::first);
	return anyOf(directClauses(c), [baseClauseNames](auto clause) {
			return contains(baseClauseNames, clause.first());
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
struct Concept : detail::ConceptBase {

	constexpr Concept() = default;

	template <class Name>
	constexpr auto getSignature(Name) const {
		constexpr auto found = anyOf(clauses(Concept{}), [](auto clause) { return clause.first() == Name{}; });
		if constexpr (found) {
			constexpr auto clause = find(clauses(Concept{}), [](auto clause) { return clause.first() == Name{}; });
			return clause.second();
		} else {
			static_assert(false, "Function not found");
		}
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
//   "dereference"_s = caramel_poly::function<Reference (caramel_poly::SelfPlaceholder&)>
//   ...
// )) { };
// ```
//
// It is recommended to make every Concept its own structure (and not just an
// alias), as above, because that ensures the uniqueness of Concepts that have
// the same clauses.
template <class... Clauses>
constexpr Concept<Clauses...> requires(Clauses...) {
	static_assert(
		!detail::hasDuplicateClause(Concept<Clauses...>{}),
		"Concept: It looks like you have multiple clauses with the same "
		"name in your Concept definition. This is not allowed; each clause must "
		"have a different name."
		);

	static_assert(
		!detail::isRedefiningBaseConceptClause(Concept<Clauses...>{}),
		"Concept: It looks like you are redefining a clause that is already "
		"defined in a base Concept. This is not allowed; clauses defined in a "
		"Concept must have a distinct name from clauses defined in base Concepts "
		"if there are any."
		);

	return {};
}

} // namespace caramel_poly

#endif // CARAMELPOLY_CONCEPT_HPP__
