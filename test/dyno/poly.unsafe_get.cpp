// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/builtin.hpp"
#include "caramel-poly/Poly.hpp"

#include <string>

namespace /* anonymous */ {

TEST(DynoTest, UnsafeGet) {
  std::string foobar{"foobar"};

  {
    // non-const version
    caramel_poly::Poly<caramel_poly::CopyConstructible> poly{foobar};
    std::string* s = poly.unsafeGet<std::string>();
    EXPECT_EQ(*s, foobar);

    // make sure the pointer we got is to the underlying storage of the poly
    *s += "baz";
    std::string* s2 = poly.unsafeGet<std::string>();
    EXPECT_EQ(*s2, "foobarbaz");
    EXPECT_EQ(*s2, *s);
  }

  {
    // const version
    caramel_poly::Poly<caramel_poly::CopyConstructible> const poly{foobar};
    std::string const* s = poly.unsafeGet<std::string>();
    EXPECT_EQ(*s, foobar);
  }
}

} // anonymous namespace
