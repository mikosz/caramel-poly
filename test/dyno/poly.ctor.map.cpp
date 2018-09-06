// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Concept.hpp"
#include "caramel-poly/ConceptMap.hpp"
#include "caramel-poly/Poly.hpp"

// This test makes sure that `caramel_poly::poly` allows overriding the concept map used
// for a type at construction time.

constexpr auto f_NAME = METHOD_NAME("f");
constexpr auto g_NAME = METHOD_NAME("g");

struct Concept : decltype(caramel_poly::requires(
  f_NAME = caramel_poly::function<int (caramel_poly::SelfPlaceholder&)>,
  g_NAME = caramel_poly::function<int (caramel_poly::SelfPlaceholder&)>
)) { };

struct Foo { };

template <class T>
auto const caramel_poly::conceptMap<Concept, T, std::enable_if_t<std::is_same_v<T, Foo>>> = caramel_poly::makeConceptMap(
  f_NAME = [](Foo&) { return 111; },
  g_NAME = [](Foo&) { return 888; }
);

TEST(DynoTest, CtorMap) {
  {
	Foo foo;
	caramel_poly::Poly<Concept> poly{foo};
	EXPECT_EQ(poly.virtual_(f_NAME)(poly), 111);
	EXPECT_EQ(poly.virtual_(g_NAME)(poly), 888);
  }

  {
    Foo foo;
    caramel_poly::Poly<Concept> poly{foo, caramel_poly::makeConceptMap(
      f_NAME = [](Foo&) { return 222; }
    )};
	EXPECT_EQ(poly.virtual_(f_NAME)(poly), 222);
	EXPECT_EQ(poly.virtual_(g_NAME)(poly), 888);
  }
}
