// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/builtin.hpp"
#include "caramel-poly/concept.hpp"
#include "caramel-poly/poly.hpp"

#include "awful.hpp"

#include <utility>

namespace /* anonymous */ {

TEST(DynoTest, CtorCopy) {
  // Make sure the copy constructor is not instantiated unless requested.
  {
    using NonCopyable = decltype(caramel_poly::requires(caramel_poly::MoveConstructible{},
                                                caramel_poly::Destructible{}));
    caramel_poly::Poly<NonCopyable> a{awful::noncopyable{}};
    caramel_poly::Poly<NonCopyable> b{std::move(a)};
  }
}

} // anonymous namespace
