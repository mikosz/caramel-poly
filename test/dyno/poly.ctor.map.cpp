// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Trait.hpp"
#include "caramel-poly/TraitMap.hpp"
#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

// This test makes sure that `caramel::poly::poly` allows overriding the trait map used
// for a type at construction time.

constexpr auto f_NAME = POLY_FUNCTION_LABEL("f");
constexpr auto g_NAME = POLY_FUNCTION_LABEL("g");

struct Trait : decltype(caramel::poly::require(
  f_NAME = caramel::poly::function<int (caramel::poly::SelfPlaceholder&)>,
  g_NAME = caramel::poly::function<int (caramel::poly::SelfPlaceholder&)>
)) { };

struct Foo { };

} // anonymous namespace

template <class T>
auto const caramel::poly::conceptMap<Trait, T, std::enable_if_t<std::is_same_v<T, Foo>>> = caramel::poly::makeTraitMap(
  f_NAME = [](Foo&) { return 111; },
  g_NAME = [](Foo&) { return 888; }
);

namespace /* anonymous */ {

TEST(DynoTest, CtorMap) {
  {
	Foo foo;
	caramel::poly::Poly<Trait> poly{foo};
	EXPECT_EQ(poly.virtual_(f_NAME)(poly), 111);
	EXPECT_EQ(poly.virtual_(g_NAME)(poly), 888);
  }

  {
    Foo foo;
    caramel::poly::Poly<Trait> poly{foo, caramel::poly::makeTraitMap(
      f_NAME = [](Foo&) { return 222; }
    )};
	EXPECT_EQ(poly.virtual_(f_NAME)(poly), 222);
	EXPECT_EQ(poly.virtual_(g_NAME)(poly), 888);
  }
}

} // anonymous namespace
