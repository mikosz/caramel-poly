// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#include <gtest/gtest.h>

#include <utility>

#include "caramel-poly/detail/DefaultConstructibleLambda.hpp"

namespace /* anonymous */ {

using namespace caramel_poly::detail;

struct Functor {
	std::pair<int, int> operator()(int arg) const {
		return { 0, arg };
	}
};

TEST(DefaultConstructibleLambdaTest, InvokesProvidedFunctionWithArguments) {
	constexpr auto functor = DefaultConstructibleLambda<Functor, std::pair<int, int> (int)>();
	EXPECT_EQ(functor(42), std::make_pair(0, 42));

	auto lambda = [](int arg) { return std::make_pair(1, arg); };
	constexpr auto lambdaFunctor = DefaultConstructibleLambda<decltype(lambda), std::pair<int, int> (int)>();
	EXPECT_EQ(lambdaFunctor(42), std::make_pair(1, 42));
}

} // anonymous namespace
