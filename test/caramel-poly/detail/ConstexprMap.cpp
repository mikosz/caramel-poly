// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/detail/ConstexprMap.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;

template <int i>
struct S {
	static constexpr auto value = i;

	constexpr S() = default;
};

template <int i>
struct T {
	static constexpr auto value = i;
};

template <int i, int j>
constexpr bool operator==(S<i>, S<j>) {
	return i == j;
}

TEST(ConstexprMap, ContainsReturnsTrueIfMapHasKey) {
	constexpr auto map = makeConstexprMap(
		makeConstexprPair(S<1>{}, T<1>{}),
		makeConstexprPair(S<2>{}, T<2>{}),
		makeConstexprPair(S<3>{}, T<3>{})
		);

	static_assert(map.contains(S<1>{}));
	static_assert(map.contains(S<2>{}));
	static_assert(map.contains(S<3>{}));
	static_assert(!map.contains(S<4>{}));
}

TEST(ConstexprMap, IndexOperatorAccessesStoredValue) {
	constexpr auto map = makeConstexprMap(
		makeConstexprPair(S<1>{}, T<1>{}),
		makeConstexprPair(S<2>{}, T<2>{}),
		makeConstexprPair(S<3>{}, T<3>{})
		);

	static_assert(std::is_same_v<decltype(map[S<1>{}]), T<1>>);
	static_assert(std::is_same_v<decltype(map[S<2>{}]), T<2>>);
	static_assert(std::is_same_v<decltype(map[S<3>{}]), T<3>>);
	// Should not compile
	//map[S<4>{}];
}

TEST(ConstexprMap, InsertAddsEntryIfKeyDoesntExist) {
	constexpr auto empty = makeConstexprMap();
	constexpr auto one = empty.insert(S<1>{}, T<1>{});
	static_assert(std::is_same_v<std::decay_t<decltype(one)>, ConstexprMap<ConstexprPair<S<1>, T<1>>>>);
	constexpr auto stillOne = one.insert(S<1>{}, T<2>{});
	static_assert(std::is_same_v<decltype(stillOne), decltype(one)>);
	constexpr auto oneTwo = one.insert(S<2>{}, T<2>{});
	static_assert(
		std::is_same_v<
			std::decay_t<decltype(oneTwo)>,
			ConstexprMap<ConstexprPair<S<2>, T<2>>, ConstexprPair<S<1>, T<1>>>
			>
		);
}

TEST(ConstexprMap, InsertAllAddsEntriesFromList) {
	constexpr auto oneTwo = makeConstexprMap(
		makeConstexprPair(S<1>{}, T<1>{}),
		makeConstexprPair(S<2>{}, T<2>{})
		);
	constexpr auto twoThree = makeConstexprList(
		makeConstexprPair(S<2>{}, T<2>{}),
		makeConstexprPair(S<3>{}, T<3>{})
		);
	constexpr auto oneTwoThree = oneTwo.insertAll(twoThree);
	static_assert(
		std::is_same_v<
			std::decay_t<decltype(oneTwoThree)>,
			ConstexprMap<
				ConstexprPair<S<3>, T<3>>,
				ConstexprPair<S<1>, T<1>>,
				ConstexprPair<S<2>, T<2>>
				>
			>
		);
}

TEST(ConstexprMap, UnionCreatesAMapContainingKeysFromBothMapsLhsValuesHavePriority) {
	constexpr auto oneTwoThree = makeConstexprMap(
		makeConstexprPair(S<1>{}, T<1>{}),
		makeConstexprPair(S<2>{}, T<2>{}),
		makeConstexprPair(S<3>{}, T<3>{})
		);
	constexpr auto twoFour = makeConstexprMap(
		makeConstexprPair(S<2>{}, T<123>{}),
		makeConstexprPair(S<4>{}, T<4>{})
		);
	constexpr auto expected = makeConstexprMap(
		makeConstexprPair(S<4>{}, T<4>{}),
		makeConstexprPair(S<1>{}, T<1>{}),
		makeConstexprPair(S<2>{}, T<2>{}),
		makeConstexprPair(S<3>{}, T<3>{})
		);
	constexpr auto got = mapUnion(oneTwoThree, twoFour);

	static_assert(std::is_same_v<decltype(got), decltype(expected)>);
}

TEST(ConstexprMap, KeysReturnsAListOfAllKeys) {
	constexpr auto oneTwoThree = makeConstexprMap(
		makeConstexprPair(S<1>{}, T<1>{}),
		makeConstexprPair(S<2>{}, T<2>{}),
		makeConstexprPair(S<3>{}, T<3>{})
		);
	constexpr auto oneTwoThreeKeys = ConstexprList<S<1>, S<2>, S<3>>{};

	static_assert(std::is_same_v<
		std::decay_t<decltype(keys(oneTwoThree))>,
		std::decay_t<decltype(oneTwoThreeKeys)>
		>);
}

} // anonymous namespace