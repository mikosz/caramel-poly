// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Trait.hpp"
#include "caramel-poly/TraitMap.hpp"
#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

// This test makes sure that `caramel::poly::poly` adds an implicit first argument
// when a function in a trait is defined using `method` instead of
// `function`.

constexpr auto a_NAME = POLY_FUNCTION_LABEL("a");
constexpr auto b_NAME = POLY_FUNCTION_LABEL("b");
constexpr auto c_NAME = POLY_FUNCTION_LABEL("c");
constexpr auto d_NAME = POLY_FUNCTION_LABEL("d");
constexpr auto e_NAME = POLY_FUNCTION_LABEL("e");

struct Trait : decltype(caramel::poly::require(
  a_NAME = caramel::poly::method<int (int)>,
  b_NAME = caramel::poly::method<int (int) &>,
  c_NAME = caramel::poly::method<int (int) &&>,
  d_NAME = caramel::poly::method<int (int) const>,
  e_NAME = caramel::poly::method<int (int) const&>
)) { };

struct Foo { };

} // anonymous namespace

template <class T>
auto const caramel::poly::conceptMap<Trait, T, std::enable_if_t<std::is_same_v<T, Foo>>> = caramel::poly::makeTraitMap(
  a_NAME = [](Foo&, int) { return 111; },
  b_NAME = [](Foo&, int) { return 222; },
  c_NAME = [](Foo&&, int) { return 333; },
  d_NAME = [](Foo const&, int) { return 444; },
  e_NAME = [](Foo const&, int) { return 555; }
);

namespace /* anonymous */ {

TEST(DynoTest, VirtualMethod) {
  {
    Foo foo;
    caramel::poly::Poly<Trait> poly{foo};
    EXPECT_EQ(poly.virtual_(a_NAME)(int{}), 111);
  }
  {
    Foo foo;
    caramel::poly::Poly<Trait> poly{foo};
	EXPECT_EQ(poly.virtual_(b_NAME)(int{}), 222);
  }
  // #TODO_Caramel: doesn't compile
 // {
 //   Foo foo;
 //   caramel::poly::Poly<Trait> poly{foo};
	//EXPECT_EQ(std::move(poly).virtual_(c_NAME)(int{}), 333);
 // }
  {
    Foo foo;
    caramel::poly::Poly<Trait> const poly{foo};
	EXPECT_EQ(poly.virtual_(d_NAME)(int{}), 444);
  }
  {
    Foo foo;
    caramel::poly::Poly<Trait> const poly{foo};
	EXPECT_EQ(poly.virtual_(e_NAME)(int{}), 555);
  }
}

} // anonymous namespace
