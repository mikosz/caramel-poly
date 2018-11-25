// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include "caramel-poly/detail/ConstexprPair.hpp"

namespace /* anonymous */ {

using namespace caramel::poly;
using namespace caramel::poly::detail;

struct Empty {
};

struct NonEmpty {
	int i;
};

TEST(ConstexprPairTest, PairOfTwoEmptiesIsEmpty) {
	auto p = ConstexprPair<Empty, Empty>{};
	static_assert(std::is_empty_v<decltype(p)>);
}

TEST(ConstexprPairTest, PairWhereSecondNonemptyHasSecond) {
	constexpr auto p = ConstexprPair<Empty, NonEmpty>{ Empty{}, NonEmpty{ 3 } };
	static_assert(sizeof(p) == sizeof(NonEmpty));
	
	static_assert(p.second().i == 3);
}

} // anonymous namespace
