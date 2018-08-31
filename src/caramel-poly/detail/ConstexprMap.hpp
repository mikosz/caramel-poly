// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#ifndef CARAMELPOLY_DETAIL_CONSTEXPRMAP_HPP__
#define CARAMELPOLY_DETAIL_CONSTEXPRMAP_HPP__

#include <type_traits>

#include "ConstexprList.hpp"
#include "ConstexprPair.hpp"

namespace caramel_poly::detail {

template <class... Entries>
class ConstexprMap;

template <class... Keys, class... Values>
class ConstexprMap<ConstexprPair<Keys, Values>...> : public ConstexprList<ConstexprPair<Keys, Values>...>
{
public:

	using Entries = ConstexprList<ConstexprPair<Keys, Values>...>;

	constexpr ConstexprMap() = default;

	template <class... Entries>
	constexpr ConstexprMap(Entries... entries) :
		ConstexprList<ConstexprPair<Keys, Values>...>(std::move(entries)...)
	{
	}

	template <class Key>
	constexpr bool contains(Key) const {
		return anyOf(static_cast<const Parent&>(*this), [](auto e) { return e.first() == Key{}; });
	}

	template <class Key>
	constexpr auto operator[](Key) const {
		constexpr auto hasEntry = ConstexprMap{}.contains(Key{});
		if constexpr (hasEntry) {
			return find(
				static_cast<const Parent&>(*this),
				[](auto e) { return e.first() == Key{}; }
				).second();
		} else {
			static_assert(hasEntry, "Map doesn't contain key");
		}
	}

	template <class Key, class Value>
	constexpr auto insert(Key, Value) const {
		if constexpr (ConstexprMap{}.contains(Key{})) {
			return ConstexprMap{};
		} else {
			return makeConstexprMap(ConstexprPair<Key, Value>{}, ConstexprPair<Keys, Values>{}...);
		}
	}

	template <class Key, class Value>
	constexpr auto insert(ConstexprPair<Key, Value>) const {
		if constexpr (ConstexprMap{}.contains(Key{})) {
			return ConstexprMap{};
		} else {
			return makeConstexprMap(ConstexprPair<Key, Value>{}, ConstexprPair<Keys, Values>{}...);
		}
	}

	template <class... OtherEntries>
	constexpr auto insertAll([[maybe_unused]] ConstexprList<OtherEntries...> other) const {
		if constexpr (empty(ConstexprMap<OtherEntries...>::Entries{})) {
			return ConstexprMap{};
		} else {
			return ConstexprMap{}
				.insert(other.head())
				.insertAll(other.tail())
				;
		}
	}

	constexpr auto entries() const {
		return static_cast<const Parent&>(*this);
	}

private:

	using Parent = ConstexprList<ConstexprPair<Keys, Values>...>;

};

template <class... Keys, class... Values>
constexpr auto makeConstexprMap(ConstexprPair<Keys, Values>... p) {
	return ConstexprMap<ConstexprPair<Keys, Values>...>{ std::move(p)... };
}

template <class... Keys, class... Values>
constexpr auto makeConstexprMap(ConstexprList<ConstexprPair<Keys, Values>...>) {
	return ConstexprMap<ConstexprPair<Keys, Values>...>{};
}

template <class... LhsEntries, class... RhsEntries>
constexpr auto mapUnion(ConstexprMap<LhsEntries...>, ConstexprMap<RhsEntries...>) {
	return ConstexprMap<LhsEntries...>{}.insertAll(ConstexprMap<RhsEntries...>::Entries{});
}

template <class... Keys, class... Values>
constexpr auto keys(ConstexprMap<ConstexprPair<Keys, Values>...>) {
	return ConstexprList<Keys...>{};
}

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_CONSTEXPRMAP_HPP__ */
