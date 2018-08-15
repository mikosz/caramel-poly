// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#ifndef CARAMELPOLY_DETAIL_CONSTEXPRLIST_HPP__
#define CARAMELPOLY_DETAIL_CONSTEXPRLIST_HPP__

#include <type_traits>

namespace caramel_poly::detail {

template <class... Tail>
class ConstexprList;

template <class Head, class... Tail>
class ConstexprList<Head, Tail...> {
public:

	constexpr ConstexprList() = default;

	constexpr auto head() const {
		return Head{};
	}

	constexpr auto tail() const {
		return ConstexprList<Tail...>{};
	}

};

template <>
class ConstexprList<> {
public:

	constexpr ConstexprList() = default;

};

template <class... Entries>
constexpr auto makeConstexprList(Entries...) {
	return ConstexprList<Entries...>{};
}

template <class NewHead, class... Entries>
constexpr auto prepend(ConstexprList<Entries...>, NewHead) {
	return ConstexprList<NewHead, Entries...>{};
}

template <class... LhsEntries, class... RhsEntries>
constexpr auto concatenate(ConstexprList<LhsEntries...>, ConstexprList<RhsEntries...>) {
	return ConstexprList<LhsEntries..., RhsEntries...>{};
}

template <class Predicate, class Head, class... Tail>
constexpr auto filter(ConstexprList<Head, Tail...> c, Predicate p) {
	if constexpr (p(Head{})) {
		return prepend(filter(c.tail(), p), Head{});
	} else {
		return filter(c.tail(), p);
	}
}

template <class Predicate>
constexpr auto filter(ConstexprList<>, Predicate) {
	return ConstexprList<>{};
}

template <class Transformer, class Head, class... Tail>
constexpr auto transform(ConstexprList<Head, Tail...> c, Transformer t) {
	return prepend(transform(c.tail(), t), t(c.head()));
}

template <class Transformer>
constexpr auto transform(ConstexprList<>, Transformer) {
	return ConstexprList<>{};
}

template <class Needle, class Head, class... Tail>
constexpr bool contains(ConstexprList<Head, Tail...> c, Needle n) {
	return c.head() == n || contains(c.tail(), n);
}

template <class Needle>
constexpr bool contains(ConstexprList<>, Needle) {
	return false;
}

template <class Head, class... Tail>
constexpr bool hasDuplicates(ConstexprList<Head, Tail...> c) {
	return contains(c.tail(), c.head()) || hasDuplicates(c.tail());
}

constexpr bool hasDuplicates(ConstexprList<>) {
	return false;
}

template <class Predicate, class Head, class... Tail>
constexpr auto anyOf(ConstexprList<Head, Tail...> c, Predicate p) {
	return p(Head{}) || anyOf(c.tail(), p);
}

template <class Predicate>
constexpr auto anyOf(ConstexprList<>, Predicate) {
	return false;
}

template <class T>
struct FlattennedConstexprList;

template <class Head, class... Tail>
struct FlattennedConstexprList<ConstexprList<Head, Tail...>> {
	using Type = decltype(prepend(typename FlattennedConstexprList<ConstexprList<Tail...>>::Type{}, Head{}));
};

template <class... HeadEntries, class... TailEntries>
struct FlattennedConstexprList<ConstexprList<ConstexprList<HeadEntries...>, TailEntries...>> {
	using Type = decltype(
		concatenate(
			typename FlattennedConstexprList<ConstexprList<HeadEntries...>>::Type{},
			typename FlattennedConstexprList<ConstexprList<TailEntries...>>::Type{}
			)
		);
};

template <>
struct FlattennedConstexprList<ConstexprList<>> {
	using Type = ConstexprList<>;
};

template <class... Entries>
constexpr auto flatten(ConstexprList<Entries...> c) {
	return typename FlattennedConstexprList<decltype(c)>::Type{};
}

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_CONSTEXPRLIST_HPP__ */
