// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#ifndef CARAMELPOLY_DETAIL_COMPILETIMEMAP_HPP__
#define CARAMELPOLY_DETAIL_COMPILETIMEMAP_HPP__

#include <type_traits>

namespace caramel_poly::detail {

template <class... Entries>
class CompileTimeMap;

template <class KeyT, class ValueT>
struct CompileTimeMapEntry {
	using Key = KeyT;
	using Value = ValueT;
};

template <class Key, class Value, class... Entries>
class CompileTimeMap<CompileTimeMapEntry<Key, Value>, Entries...> {
public:

	template <class AKey>
	constexpr static bool contains() {
		return std::is_same_v<AKey, Key> || CompileTimeMap<Entries...>::contains<AKey>();
	}

	constexpr static bool hasDuplicates() {
		return CompileTimeMap<Entries...>::contains<Key>() || CompileTimeMap<Entries...>::hasDuplicates();
	}

};

template <>
class CompileTimeMap<> {
public:

	template <class AKey>
	constexpr static bool contains() {
		return false;
	}

	constexpr static bool hasDuplicates() {
		return false;
	}

};

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_COMPILETIMEMAP_HPP__ */
