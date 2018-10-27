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

	constexpr ConstexprMap(ConstexprList<ConstexprPair<Keys, Values>...> entries) :
		ConstexprList<ConstexprPair<Keys, Values>...>(std::move(entries))
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
	constexpr auto insert(Key k, Value v) const {
		return insert(makeConstexprPair(std::move(k), std::move(v)));
	}

	template <class Key, class Value>
	constexpr auto insert([[maybe_unused]] ConstexprPair<Key, Value> p) const {
		if constexpr (ConstexprMap{}.contains(Key{})) {
			return *this;
		} else {
			return ConstexprMap<ConstexprPair<Key, Value>, ConstexprPair<Keys, Values>...>{
				prepend(entries(), std::move(p))
				};
		}
	}

	template <class... OtherEntries>
	constexpr auto insertAll([[maybe_unused]] ConstexprList<OtherEntries...> other) const {
		if constexpr (empty(typename ConstexprMap<OtherEntries...>::Entries{})) {
			return *this;
		} else {
			return
				insert(other.head())
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
constexpr auto makeConstexprMap(ConstexprList<ConstexprPair<Keys, Values>...> e) {
	return ConstexprMap<ConstexprPair<Keys, Values>...>{ std::move(e) };
}

template <class... LhsEntries, class... RhsEntries>
constexpr auto mapUnion(ConstexprMap<LhsEntries...> lhs, ConstexprMap<RhsEntries...> rhs) {
	return lhs.insertAll(rhs.entries());
}

template <class... Keys, class... Values>
constexpr auto keys(ConstexprMap<ConstexprPair<Keys, Values>...>) {
	return ConstexprList<Keys...>{};
}

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_CONSTEXPRMAP_HPP__ */
