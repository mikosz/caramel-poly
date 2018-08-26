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
class ConstexprMap<ConstexprPair<Keys, Values>...> {
public:

	using Entries = ConstexprList<ConstexprPair<Keys, Values>...>;

	constexpr ConstexprMap() = default;

	template <class Key>
	constexpr bool contains(Key) const {
		return anyOf(Entries{}, [](auto e) { return e.first() == Key{}; });
	}

	template <class Key>
	constexpr auto operator[](Key) const {
		constexpr auto hasEntry = ConstexprMap{}.contains(Key{});
		if constexpr (hasEntry) {
			return find(Entries{}, [](auto e) { return e.first() == Key{}; }).second();
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
	constexpr auto insert(ConstexprPair<Key, Value> p) const {
		return insert(p.first(), p.second());
	}

	template <class... OtherEntries>
	constexpr auto insertAll(ConstexprList<OtherEntries...>) const {
		if constexpr (empty(ConstexprMap<OtherEntries...>::Entries{})) {
			return ConstexprMap{};
		} else {
			return ConstexprMap{}
				.insert(ConstexprList<OtherEntries...>{}.head())
				.insertAll(ConstexprList<OtherEntries...>{}.tail())
				;
		}
	}

};

template <class... Keys, class... Values>
constexpr auto makeConstexprMap(ConstexprPair<Keys, Values>...) {
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
