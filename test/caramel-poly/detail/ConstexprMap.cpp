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

template <int i, int j>
constexpr bool operator==(S<i>, S<j>) {
	return i == j;
}

template <int i>
struct T {
	static constexpr auto value = i;
};

struct V {
	int i;
};

TEST(ConstexprMap, MapOfEmptyElementsHasSize1) {
	constexpr auto oneTwoThreeEmpty = makeConstexprMap(
		makeConstexprPair(S<1>{}, T<1>{}),
		makeConstexprPair(S<2>{}, T<2>{}),
		makeConstexprPair(S<3>{}, T<3>{})
		);
	static_assert(sizeof(oneTwoThreeEmpty) == 1);
}

TEST(ConstexprMap, MapOfNonemptyElementsHasSizeOfSumOfElements) {
	// This may not be true in cases where elements are of different sizes, due
	// to padding, but this is fine for our needs
	constexpr auto oneTwoThreeNonempty = makeConstexprMap(
		makeConstexprPair(S<1>{}, 1.0f),
		makeConstexprPair(S<2>{}, 2.0f),
		makeConstexprPair(S<3>{}, 3.0f)
		);
	static_assert(sizeof(oneTwoThreeNonempty) == 3 * sizeof(float));
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

TEST(ConstexprMap, IndexOperatorAccessesStoredEmptyValue) {
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

TEST(ConstexprMap, IndexOperatorAccessesStoredNonemptyValue) {
	constexpr auto map = makeConstexprMap(
		makeConstexprPair(S<1>{}, V{ 1 }),
		makeConstexprPair(S<2>{}, V{ 2 }),
		makeConstexprPair(S<3>{}, V{ 3 })
		);

	EXPECT_EQ(map[S<1>{}].i, 1);
	EXPECT_EQ(map[S<2>{}].i, 2);
	EXPECT_EQ(map[S<3>{}].i, 3);
	// Should not compile
	//map[S<4>{}];
}

TEST(ConstexprMap, InsertAddsEntryIfKeyDoesntExist) {
	{
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

	{
		constexpr auto empty = makeConstexprMap();
		constexpr auto one = empty.insert(S<1>{}, V{ 1 });
		constexpr auto oneTwo = one.insert(S<2>{}, V{ 2 });
		static_assert(oneTwo[S<1>{}].i == 1);
		static_assert(oneTwo[S<2>{}].i == 2);
	}
}

TEST(ConstexprMap, InsertAllAddsEntriesFromList) {
	constexpr auto oneTwoEmpty = makeConstexprMap(
		makeConstexprPair(S<1>{}, T<1>{}),
		makeConstexprPair(S<2>{}, T<2>{})
		);
	constexpr auto twoThreeEmpty = makeConstexprList(
		makeConstexprPair(S<2>{}, T<2>{}),
		makeConstexprPair(S<3>{}, T<3>{})
		);
	constexpr auto oneTwoThreeEmpty = oneTwoEmpty.insertAll(twoThreeEmpty);
	static_assert(
		std::is_same_v<
			std::decay_t<decltype(oneTwoThreeEmpty)>,
			ConstexprMap<
				ConstexprPair<S<3>, T<3>>,
				ConstexprPair<S<1>, T<1>>,
				ConstexprPair<S<2>, T<2>>
				>
			>
		);

	constexpr auto oneTwoNonempty = makeConstexprMap(
		makeConstexprPair(S<1>{}, 1),
		makeConstexprPair(S<2>{}, 2.0)
		);
	constexpr auto twoThreeNonempty = makeConstexprList(
		makeConstexprPair(S<2>{}, '2'),
		makeConstexprPair(S<3>{}, '3')
		);
	constexpr auto oneTwoThreeNonempty = oneTwoNonempty.insertAll(twoThreeNonempty);
	static_assert(oneTwoThreeNonempty[S<1>{}] == 1);
	static_assert(oneTwoThreeNonempty[S<2>{}] == 2.0);
	static_assert(oneTwoThreeNonempty[S<3>{}] == '3');
}

TEST(ConstexprMap, UnionCreatesAMapContainingKeysFromBothMapsLhsValuesHavePriority) {
	{
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

	{
		constexpr auto oneTwoThree = makeConstexprMap(
			makeConstexprPair(S<1>{}, 1),
			makeConstexprPair(S<2>{}, 2),
			makeConstexprPair(S<3>{}, 3)
			);
		constexpr auto twoFour = makeConstexprMap(
			makeConstexprPair(S<2>{}, 123),
			makeConstexprPair(S<4>{}, 4)
			);
		constexpr auto got = mapUnion(oneTwoThree, twoFour);

		static_assert(got[S<1>{}] == 1);
		static_assert(got[S<2>{}] == 2);
		static_assert(got[S<3>{}] == 3);
		static_assert(got[S<4>{}] == 4);
	}
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
