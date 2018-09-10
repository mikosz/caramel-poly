// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/builtin.hpp"
#include "caramel-poly/Poly.hpp"

#include <typeinfo>

// This test makes sure that we do not double-wrap `poly`s due to the implicit
// constructors, despite the fact that the poly models the concept that it
// itself requires (and hence would be a valid candidate for the implicit
// converting constructors).

namespace /* anonymous */ {

struct Foo { };

struct Concept
	: decltype(caramel_poly::requires(caramel_poly::CopyConstructible{}, caramel_poly::TypeId{}))
{ };

TEST(DynoTest, CtorImplicitDoubleWrap) {
  Foo foo;
  caramel_poly::Poly<Concept> poly{foo};

  // We expect the objects below to be a copy of the above `poly` (and thus
  // `poly`s holding a `Foo`), not `poly`s holding a `poly`.
  const auto si = poly.virtual_(caramel_poly::STORAGE_INFO_LABEL)();
  EXPECT_EQ(si.size, sizeof(Foo));

  caramel_poly::Poly<Concept> explicit_copy{poly};
  EXPECT_EQ(explicit_copy.virtual_(caramel_poly::TYPEID_LABEL)(), typeid(Foo));

  caramel_poly::Poly<Concept> implicit_copy = poly;
  EXPECT_EQ(implicit_copy.virtual_(caramel_poly::TYPEID_LABEL)(), typeid(Foo));
}

} // anonymous namespace
