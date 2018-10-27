// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#ifndef CARAMELPOLY_DETAIL_CONSTEXPRLIST_HPP__
#define CARAMELPOLY_DETAIL_CONSTEXPRLIST_HPP__

#include <type_traits>

namespace caramel_poly::detail {

template <class T, class P, class = void>
struct ListStorage;

template <class T, class P>
struct ListStorage<T, P, std::enable_if_t<std::is_empty_v<T>>> : P {

	constexpr ListStorage() = default;

	template <class... ParentArgs>
	constexpr ListStorage(T, ParentArgs... parentArgs) :
		P(std::move(parentArgs)...)
	{
	}

	constexpr T data() const {
		return T{};
	}

};

template <class T, class P>
struct ListStorage<T, P, std::enable_if_t<!std::is_empty_v<T>>> : P {

	T data_;

	constexpr ListStorage() = default;

	template <class... ParentArgs>
	constexpr ListStorage(T data, ParentArgs... parentArgs) :
		P(std::move(parentArgs)...),
		data_(std::move(data))
	{
	}

	constexpr ListStorage(T data, const P& parent) :
		P(parent),
		data_(std::move(data))
	{
	}

	constexpr T data() const {
		return data_;
	}

};

template <class... Tail>
class ConstexprList;

template <class Head, class... Tail>
class ConstexprList<Head, Tail...> : /* private */ ListStorage<Head, ConstexprList<Tail...>> {
public:

	constexpr ConstexprList() = default;

	constexpr ConstexprList(Head h, Tail... t) :
		Parent(std::move(h), std::move(t)...)
	{
	}

	constexpr ConstexprList(Head h, const ConstexprList<Tail...>& t) :
		Parent(std::move(h), t)
	{
	}

	constexpr auto head() const {
		return Parent::data();
	}

	constexpr decltype(auto) tail() const {
		return static_cast<const ConstexprList<Tail...>&>(*this);
	}

private:

	using Parent = ListStorage<Head, ConstexprList<Tail...>>;

};

template <>
class ConstexprList<> {
public:

	constexpr ConstexprList() = default;

};

template <class... Entries>
constexpr auto makeConstexprList(Entries... e) {
	return ConstexprList<Entries...>{ std::move(e)... };
}

template <class... Entries>
constexpr bool empty(ConstexprList<Entries...>) {
	return sizeof...(Entries) == 0;
}

template <class NewHead, class... Entries>
constexpr auto prepend(const ConstexprList<Entries...>& l, NewHead h) {
	return ConstexprList<NewHead, Entries...>{ std::move(h), l };
}

template <class LhsHead, class... LhsTail, class... RhsEntries>
constexpr auto concatenate(ConstexprList<LhsHead, LhsTail...> lhs, ConstexprList<RhsEntries...> rhs) {
	return prepend(concatenate(lhs.tail(), rhs), lhs.head());
}

template <class... RhsEntries>
constexpr auto concatenate(ConstexprList<>, ConstexprList<RhsEntries...> rhs) {
	return rhs;
}

template <class Head, class... Tail, class MetaPredicate>
constexpr auto find([[maybe_unused]] ConstexprList<Head, Tail...> c, MetaPredicate p) {
	if constexpr (p(Head{})) {
		return c.head();
	} else {
		return find(c.tail(), p);
	}
}

template <class MetaPredicate>
constexpr auto find(ConstexprList<>, MetaPredicate) {
	// static_assert(false, "Element not found");
}

template <class Head, class... Tail, class MetaPredicate>
constexpr auto filter(ConstexprList<Head, Tail...> c, MetaPredicate p) {
	if constexpr (p(Head{})) {
		return prepend(filter(c.tail(), p), c.head());
	} else {
		return filter(c.tail(), p);
	}
}

template <class MetaPredicate>
constexpr auto filter(ConstexprList<>, MetaPredicate) {
	return ConstexprList<>{};
}

template <class Function, class Head, class... Tail>
constexpr void forEach(ConstexprList<Head, Tail...> l, Function f) {
	f(l.head());
	forEach(l.tail(), f);
}

template <class Function>
constexpr void forEach(ConstexprList<>, Function) {
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

template <class Predicate, class... Entries>
constexpr auto allOf(ConstexprList<Entries...> l, Predicate p) {
	return !anyOf(l, [p](auto e) { return !p(e); });
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

template <class State, class Foldable, class Function>
constexpr auto foldLeft(State state, [[maybe_unused]] Foldable foldable, [[maybe_unused]] Function f) {
	if constexpr (empty(Foldable{})) {
		return state;
	} else {
		return foldLeft(f(state, foldable.head()), foldable.tail(), f);
	}
}

template <class Foldable, class Function>
constexpr auto foldLeft(Foldable foldable, [[maybe_unused]] Function f) {
	if constexpr (empty(Foldable{})) {
		// static_assert(false, "Can't fold an empty list");
	} else if constexpr (empty(Foldable{}.tail())) {
		return foldable.head();
	} else {
		return foldLeft(foldable.head(), foldable.tail(), f);
	}
}

template <class F, class... Entries>
constexpr auto unpack(ConstexprList<Entries...>, F f) {
	return f(Entries{}...);
}

template <class... LhsEntries, class... RhsEntries>
constexpr auto isSubset(ConstexprList<LhsEntries...> l, ConstexprList<RhsEntries...> of) {
	return allOf(l, [of](auto e) { return contains(of, e); });
}

template <class... LhsEntries, class... RhsEntries>
constexpr auto difference(ConstexprList<LhsEntries...> lhs, ConstexprList<RhsEntries...> rhs) {
	return foldLeft(
		ConstexprList<>{},
		lhs,
		[rhs](auto d, auto e) {
				if constexpr (contains(decltype(rhs){}, decltype(e){})) {
					return d;
				} else {
					return prepend(d, e);
				}
			});
}

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_CONSTEXPRLIST_HPP__ */
