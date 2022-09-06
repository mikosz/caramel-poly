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

namespace caramel::poly {

template <class... Clauses>
struct Trait;

namespace detail {

struct TraitBase {
};

template <class Name, class Signature>
constexpr auto expandClauses(const ConstexprPair<Name, Signature>&) {
	return ConstexprPair<Name, Signature>{};
}

template <typename... Clauses>
constexpr auto expandClauses(const Trait<Clauses...>&) {
	return flatten(makeConstexprList(expandClauses(Clauses{})...));
}

// Returns a sequence containing all the clauses of the given Trait and
// its derived Traits.
//
// In the returned sequence, each clause is a pair where the first element
// is the name of the clause and the second element is the clause itself
// (e.g. a `caramel::poly::function`). The order of clauses is not specified.
template <class... Clauses>
constexpr auto clauses(const Trait<Clauses...>&) {
	return flatten(makeConstexprList(detail::expandClauses(Clauses{})...));
}

// Returns a sequence containing the names associated to all the claused of
// the given trait, and its derived concepts.
//
// The order of the clause names is not specified.
template <class... Clauses>
constexpr auto clauseNames(const Trait<Clauses...>& c) {
	return transform(clauses(c), detail::first);
}

// Returns a sequence of the Traits refined (extended) by the given Trait.
//
// Only the Traits that are refined directly by `c` are returned, i.e. we
// do not get the refined Traits of the refined Traits recursively.
template <class... Clauses>
constexpr auto refinedTraits(const Trait<Clauses...>&) {
	return filter(detail::makeConstexprList(Clauses{}...), [](auto t) {
			return std::is_base_of_v<detail::TraitBase, decltype(t)>;
		});
}

template <class... Clauses>
constexpr auto directClauses(const Trait<Clauses...>&) {
	return filter(detail::makeConstexprList(Clauses{}...), [](auto t) {
			return !std::is_base_of_v<detail::TraitBase, decltype(t)>;
		});
}

template <class... Clauses>
constexpr auto hasDuplicateClause(const Trait<Clauses...>& c) {
	return hasDuplicates(transform(directClauses(c), detail::first));
}

template <class... Clauses>
constexpr auto isRedefiningBaseTraitClause(const Trait<Clauses...>& c) {
	auto bases = refinedTraits(c);
	auto baseClauses = flatten(transform(bases, [](const auto c) { return clauses(c); }));
	auto baseClauseNames = transform(baseClauses, detail::first);
	return anyOf(directClauses(c), [baseClauseNames](auto clause) {
			return contains(baseClauseNames, clause.first());
		});
}

} // namespace detail

// A `Trait` is a collection of clauses and refined Traits representing
// requirements for a type to model the Trait.
//
// A Trait is created by using `caramel::poly::require`.
//
// From a `Trait`, one can generate a virtual function table by looking at
// the signatures of the functions defined in the Trait. In the future, it
// would also be possible to do much more, like getting a predicate that checks
// whether a type satisfies the Trait.
template <class... Clauses>
struct Trait : detail::TraitBase {

	constexpr Trait() = default;

	template <class Name>
	constexpr auto getSignature(Name) const {
		constexpr auto found = anyOf(clauses(Trait{}), [](auto clause) { return clause.first() == Name{}; });
		if constexpr (found) {
			constexpr auto clause = find(clauses(Trait{}), [](auto clause) { return clause.first() == Name{}; });
			return clause.second();
		} else {
			// static_assert(false, "Function not found");
		}
	}

};

// Creates a `Trait` with the given clauses. Note that a clause may be a
// Trait itself, in which case the clauses of that Trait are used, and
// that, recursively. For example:
//
// ```
// template <class Reference>
// struct Iterator : decltype(caramel::poly::require(
//   Incrementable{},
//   "dereference"_s = caramel::poly::function<Reference (caramel::poly::SelfPlaceholder&)>
//   ...
// )) { };
// ```
//
// It is recommended to make every Trait its own structure (and not just an
// alias), as above, because that ensures the uniqueness of Traits that have
// the same clauses.
template <class... Clauses>
constexpr Trait<Clauses...> require(Clauses...) {
	static_assert(
		!detail::hasDuplicateClause(Trait<Clauses...>{}),
		"Trait: It looks like you have multiple clauses with the same "
		"name in your Trait definition. This is not allowed; each clause must "
		"have a different name."
		);

	static_assert(
		!detail::isRedefiningBaseTraitClause(Trait<Clauses...>{}),
		"Trait: It looks like you are redefining a clause that is already "
		"defined in a base Trait. This is not allowed; clauses defined in a "
		"Trait must have a distinct name from clauses defined in base Traits "
		"if there are any."
		);

	return {};
}

} // namespace caramel::poly

#endif // CARAMELPOLY_CONCEPT_HPP__
