// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/detail/ConstexprList.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;

template <int i>
struct S {
	static constexpr auto value = i;
};

template <int i>
struct T {
	static constexpr auto value = i;
};

template <int i, int j>
constexpr bool operator==(S<i>, S<j>) {
	return i == j;
}

TEST(ConstexprListTest, PrependPutsElementsInFront) {
	constexpr auto oneTwoThree = prepend(prepend(prepend(ConstexprList<>{}, S<3>{}), S<2>{}), S<1>{});
	static_assert(std::is_same_v<std::decay_t<decltype(oneTwoThree)>, ConstexprList<S<1>, S<2>, S<3>>>);
}

TEST(ConstexprListTest, MakeConstexprListInsertsAllElements) {
	constexpr auto oneTwoThreeOne = makeConstexprList(S<1>{}, S<2>{}, S<3>{}, S<1>{});
	static_assert(std::is_same_v<std::decay_t<decltype(oneTwoThreeOne)>, ConstexprList<S<1>, S<2>, S<3>, S<1>>>);
}

TEST(ConstexprListTest, ConcatenateMergesTwoLists) {
	constexpr auto oneTwoThreeFour = concatenate(ConstexprList<S<1>, S<2>>{}, ConstexprList<S<3>, S<4>>{});
	static_assert(std::is_same_v<std::decay_t<decltype(oneTwoThreeFour)>, ConstexprList<S<1>, S<2>, S<3>, S<4>>>);
}

TEST(ConstexprListTest, FindReturnsMatchingElement) {
	constexpr auto oneTwoThree = ConstexprList<S<1>, S<2>, S<3>>{};
	static_assert(find(oneTwoThree, [](auto e) { return e == S<1>{}; }) == S<1>{});
	static_assert(find(oneTwoThree, [](auto e) { return e == S<2>{}; }) == S<2>{});
	static_assert(find(oneTwoThree, [](auto e) { return e == S<3>{}; }) == S<3>{});
	// Should not (and does not) compile
	// static_assert(find(oneTwoThree, [](auto e) { return e == S<4>{}; }) == S<4>{});
}

TEST(ConstexprListTest, FilterGetsElementsSatisfyingPredicate) {
	constexpr auto oneTwoThreeFour = makeConstexprList(S<1>{}, S<2>{}, S<3>{}, S<4>{});
	constexpr auto twoFour = filter(oneTwoThreeFour, [](auto s) {
			return s.value % 2 == 0;
		});
	static_assert(std::is_same_v<std::decay_t<decltype(twoFour)>, ConstexprList<S<2>, S<4>>>);
}

TEST(ConstexprListTest, TransformReturnsListWithTransformedElements) {
	constexpr auto sOneTwoThree = makeConstexprList(S<1>{}, S<2>{}, S<3>{});
	constexpr auto tOneTwoThree = transform(sOneTwoThree, [](auto s) {
			return T<decltype(s)::value>{};
		});
	static_assert(std::is_same_v<std::decay_t<decltype(tOneTwoThree)>, ConstexprList<T<1>, T<2>, T<3>>>);
}

TEST(ConstexprListTest, ContainsReturnsTrueIfListContainsElement) {
	constexpr auto oneTwoThree = makeConstexprList(S<1>{}, S<2>{}, S<3>{});
	static_assert(contains(oneTwoThree, S<1>{}));
	static_assert(contains(oneTwoThree, S<2>{}));
	static_assert(contains(oneTwoThree, S<3>{}));
	static_assert(!contains(oneTwoThree, S<4>{}));
}

TEST(ConstexprListTest, EmptyReturnsTrueIfListIsEmpty) {
	constexpr auto nothing = makeConstexprList();
	static_assert(empty(nothing));
	constexpr auto oneTwoThree = makeConstexprList(S<1>{}, S<2>{}, S<3>{});
	static_assert(!empty(oneTwoThree));
}

TEST(ConstexprListTest, HasDuplicatesReturnsTrueIfListContainsDuplicates) {
	constexpr auto withoutDuplicates = ConstexprList<S<1>, S<2>, S<3>>{};
	static_assert(!hasDuplicates(withoutDuplicates));
	static_assert(hasDuplicates(concatenate(withoutDuplicates, ConstexprList<S<1>>{})));
	static_assert(hasDuplicates(concatenate(withoutDuplicates, ConstexprList<S<2>>{})));
	static_assert(hasDuplicates(concatenate(withoutDuplicates, ConstexprList<S<3>>{})));
}

TEST(ConstexprListTest, AnyOfReturnsTrueIfElementsSatisfyingPredicateExists) {
	constexpr auto oneTwoThree = makeConstexprList(S<1>{}, S<2>{}, S<3>{});

	static_assert(anyOf(oneTwoThree, [](auto s) { return s.value == 1; }));
	static_assert(anyOf(oneTwoThree, [](auto s) { return s.value == 2; }));
	static_assert(anyOf(oneTwoThree, [](auto s) { return s.value == 3; }));
	static_assert(!anyOf(oneTwoThree, [](auto s) { return s.value == 4; }));
}

TEST(ConstexprListTest, AllOfReturnsTrueIfNoElementNotSatisfyingPredicateExists) {
	constexpr auto oneTwoThree = makeConstexprList(S<1>{}, S<2>{}, S<3>{});

	static_assert(allOf(oneTwoThree, [](auto s) { return s.value < 4; }));
	static_assert(!allOf(oneTwoThree, [](auto s) { return s.value < 3; }));
}

TEST(ConstexprListTest, FlattenCreatesAFlatList) {
	constexpr auto nested =
		ConstexprList<
			S<1>,
			ConstexprList<
				ConstexprList<S<2>, S<3>>,
				S<4>
				>,
			S<5>
		>{};
	static_assert(std::is_same_v<std::decay_t<decltype(flatten(nested))>, ConstexprList<S<1>, S<2>, S<3>, S<4>, S<5>>>);
}

TEST(ConstexprListTest, FoldLeftCallsFunctorFoldingLeft) {
	constexpr auto l = makeConstexprList(S<1>{}, S<2>{}, S<3>{});
	constexpr auto v =
		foldLeft(l, [](auto lhs, auto rhs) { return S<decltype(lhs)::value * 10 + decltype(rhs)::value>{}; });
	static_assert(std::is_same_v<std::decay_t<decltype(v)>, S<123>>);
}

TEST(ConstexprListTest, FoldLeftReturnsHeadIfListHasSingleElement) {
	constexpr auto l = makeConstexprList(S<1>{});
	constexpr auto v = foldLeft(l, []() {});
	static_assert(std::is_same_v<std::decay_t<decltype(v)>, S<1>>);

	// Should fail with message "Can't fold an empty list"
	//foldLeft(ConstexprList<>{}, []() {});
}

TEST(ConstexprListTest, UnpackCallsFunctionPassingAllElements) {
	constexpr auto l = makeConstexprList(S<1>{}, S<2>{}, S<3>{});
	constexpr auto r = unpack(l, [](S<1>, S<2>, S<3>) { return 42; });
	static_assert(r == 42);
}

TEST(ConstexprListTest, IsSubsetReturnsTrueIfRhsContainsAllElementsOfLhs) {
	constexpr auto sub = makeConstexprList(S<1>{}, S<2>{}, S<3>{});
	constexpr auto super = makeConstexprList(S<3>{}, S<1>{}, S<2>{}, S<4>{});

	static_assert(isSubset(sub, sub));
	static_assert(isSubset(sub, super));
	static_assert(!isSubset(super, sub));
}

TEST(ConstexprListTest, DifferenceReturnsLhsMinusRhs) {
	constexpr auto lhs = makeConstexprList(S<1>{}, S<2>{}, S<6>{}, S<3>{}, S<5>{});
	constexpr auto rhs = makeConstexprList(S<3>{}, S<1>{}, S<2>{}, S<4>{});
	constexpr auto d = difference(lhs, rhs);

	static_assert(std::is_same_v<
		std::decay_t<decltype(d)>,
		ConstexprList<S<5>, S<6>>
		>);
}

} // anonymous namespace
