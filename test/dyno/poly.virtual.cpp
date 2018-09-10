// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Concept.hpp"
#include "caramel-poly/ConceptMap.hpp"
#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

constexpr auto a_NAME = METHOD_NAME("a");
constexpr auto b_NAME = METHOD_NAME("b");
constexpr auto c_NAME = METHOD_NAME("c");
constexpr auto d_NAME = METHOD_NAME("d");
constexpr auto e_NAME = METHOD_NAME("e");

// This test makes sure that `caramel_poly::poly` does the right thing when calling
// virtual functions with placeholders. The expected behavior is that we
// pass a `caramel_poly::poly`, and it gets translated to a `void*` internally.

struct Concept : decltype(caramel_poly::requires(
  a_NAME = caramel_poly::function<int (caramel_poly::SelfPlaceholder&)>,
  b_NAME = caramel_poly::function<int (caramel_poly::SelfPlaceholder&&)>,
  c_NAME = caramel_poly::function<int (caramel_poly::SelfPlaceholder*)>,
  d_NAME = caramel_poly::function<int (caramel_poly::SelfPlaceholder const&)>,
  e_NAME = caramel_poly::function<int (caramel_poly::SelfPlaceholder const*)>
)) { };

struct Foo { };

} // anonymous namespace

template <class T>
auto const caramel_poly::conceptMap<Concept, T, std::enable_if_t<std::is_same_v<Foo, T>>> = caramel_poly::makeConceptMap(
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
    caramel_poly::Poly<Concept> poly{foo};
    EXPECT_EQ(poly.virtual_(a_NAME)(poly), 111);
  }
  {
    Foo foo;
    caramel_poly::Poly<Concept> poly{foo};
	EXPECT_EQ(poly.virtual_(b_NAME)(std::move(poly)), 222);
  }
  {
    Foo foo;
    caramel_poly::Poly<Concept> poly{foo};
	EXPECT_EQ(poly.virtual_(c_NAME)(&poly), 333);
  }
  {
    Foo foo;
    caramel_poly::Poly<Concept> const poly{foo};
	EXPECT_EQ(poly.virtual_(d_NAME)(poly), 444);
  }
  {
    Foo foo;
    caramel_poly::Poly<Concept> const poly{foo};
	EXPECT_EQ(poly.virtual_(e_NAME)(&poly), 555);
  }
}

} // anonymous namespace
