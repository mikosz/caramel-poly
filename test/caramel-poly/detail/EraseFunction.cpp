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

TEST(EraseFunctionTest, ErasesPlaceholdersFromLambda) {
	auto foo = []() {};
	auto defaultConstructible = DefaultConstructibleLambda<decltype(foo), void ()>;

}

} // anonymous namespace
