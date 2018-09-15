// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Concept.hpp"
#include "caramel-poly/ConceptMap.hpp"
#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

// This test makes sure that `caramel_poly::poly` adds an implicit first argument
// when a function in a concept is defined using `method` instead of
// `function`.

constexpr auto a_NAME = POLY_FUNCTION_LABEL("a");
constexpr auto b_NAME = POLY_FUNCTION_LABEL("b");
constexpr auto c_NAME = POLY_FUNCTION_LABEL("c");
constexpr auto d_NAME = POLY_FUNCTION_LABEL("d");
constexpr auto e_NAME = POLY_FUNCTION_LABEL("e");

struct Concept : decltype(caramel_poly::requires(
  a_NAME = caramel_poly::method<int (int)>,
  b_NAME = caramel_poly::method<int (int) &>,
  c_NAME = caramel_poly::method<int (int) &&>,
  d_NAME = caramel_poly::method<int (int) const>,
  e_NAME = caramel_poly::method<int (int) const&>
)) { };

struct Foo { };

} // anonymous namespace

template <class T>
auto const caramel_poly::conceptMap<Concept, T, std::enable_if_t<std::is_same_v<T, Foo>>> = caramel_poly::makeConceptMap(
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
    caramel_poly::Poly<Concept> poly{foo};
    EXPECT_EQ(poly.virtual_(a_NAME)(int{}), 111);
  }
  {
    Foo foo;
    caramel_poly::Poly<Concept> poly{foo};
	EXPECT_EQ(poly.virtual_(b_NAME)(int{}), 222);
  }
  // #TODO_Caramel
 // {
 //   Foo foo;
 //   caramel_poly::Poly<Concept> poly{foo};
	//EXPECT_EQ(std::move(poly).virtual_(c_NAME)(int{}), 333);
 // }
  {
    Foo foo;
    caramel_poly::Poly<Concept> const poly{foo};
	EXPECT_EQ(poly.virtual_(d_NAME)(int{}), 444);
  }
  {
    Foo foo;
    caramel_poly::Poly<Concept> const poly{foo};
	EXPECT_EQ(poly.virtual_(e_NAME)(int{}), 555);
  }
}

} // anonymous namespace
