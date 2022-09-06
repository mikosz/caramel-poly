// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Trait.hpp"
#include "caramel-poly/TraitMap.hpp"
#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

constexpr auto a_NAME = POLY_FUNCTION_LABEL("a");
constexpr auto b_NAME = POLY_FUNCTION_LABEL("b");
constexpr auto c_NAME = POLY_FUNCTION_LABEL("c");
constexpr auto d_NAME = POLY_FUNCTION_LABEL("d");
constexpr auto e_NAME = POLY_FUNCTION_LABEL("e");

// This test makes sure that `caramel::poly::poly` does the right thing when calling
// virtual functions with placeholders. The expected behavior is that we
// pass a `caramel::poly::poly`, and it gets translated to a `void*` internally.

struct Trait : decltype(caramel::poly::require(
  a_NAME = caramel::poly::function<int (caramel::poly::SelfPlaceholder&)>,
  b_NAME = caramel::poly::function<int (caramel::poly::SelfPlaceholder&&)>,
  c_NAME = caramel::poly::function<int (caramel::poly::SelfPlaceholder*)>,
  d_NAME = caramel::poly::function<int (caramel::poly::SelfPlaceholder const&)>,
  e_NAME = caramel::poly::function<int (caramel::poly::SelfPlaceholder const*)>
)) { };

struct Foo { };

} // anonymous namespace

template <class T>
auto const caramel::poly::conceptMap<Trait, T, std::enable_if_t<std::is_same_v<Foo, T>>> = caramel::poly::makeTraitMap(
  a_NAME = [](Foo&) { return 111; },
  b_NAME = [](Foo&&) { return 222; },
  c_NAME = [](Foo*) { return 333; },
  d_NAME = [](Foo const&) { return 444; },
  e_NAME = [](Foo const*) { return 555; }
);

namespace /* anonymous */ {

TEST(DynoTest, Virtual) {
  {
    Foo foo;
    caramel::poly::Poly<Trait> poly{foo};
    EXPECT_EQ(poly.virtual_(a_NAME)(poly), 111);
  }
  {
    Foo foo;
    caramel::poly::Poly<Trait> poly{foo};
	EXPECT_EQ(poly.virtual_(b_NAME)(std::move(poly)), 222);
  }
  {
    Foo foo;
    caramel::poly::Poly<Trait> poly{foo};
	EXPECT_EQ(poly.virtual_(c_NAME)(&poly), 333);
  }
  {
    Foo foo;
    caramel::poly::Poly<Trait> const poly{foo};
	EXPECT_EQ(poly.virtual_(d_NAME)(poly), 444);
  }
  {
    Foo foo;
    caramel::poly::Poly<Trait> const poly{foo};
	EXPECT_EQ(poly.virtual_(e_NAME)(&poly), 555);
  }
}

} // anonymous namespace
