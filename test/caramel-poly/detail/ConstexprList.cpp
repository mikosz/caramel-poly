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

} // anonymous namespace
