// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/detail/EraseFunction.hpp"
#include "caramel-poly/detail/DefaultConstructibleLambda.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;

struct S {
	int i;
};

TEST(EraseFunctionTest, ErasesPlaceholdersFromLambda) {
	auto foo = [](const S& s1, S& s2, int i) -> S* {
			s2.i = s1.i * i;
			return &s2;
		};
	const auto thunk = EraseFunction<const SelfPlaceholder* (const SelfPlaceholder&, SelfPlaceholder&, int)>(foo);

	auto s1 = S{ 42 };
	auto s2 = S{};
	auto* r = (*thunk)(&s1, &s2, 2);

	EXPECT_EQ(s2.i, 84);
	EXPECT_EQ(r, &s2);
}

} // anonymous namespace
