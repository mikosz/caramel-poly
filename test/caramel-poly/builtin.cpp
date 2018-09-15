// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <string>

#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

struct S {
};

} // anonymous namespace

template <class T>
constexpr auto caramel_poly::conceptMap<TypeId, T, std::enable_if_t<std::is_same_v<S, T>>> = makeConceptMap(
	);

namespace /* anonymous */ {

TEST(PolyTest, TypeIdAccessible) {
	auto sp = Poly<TypeId>{S{}};
	EXPECT_EQ(sp.virtual_(TYPEID_LABEL)(), typeid(S));
}

} // anonymous namespace
