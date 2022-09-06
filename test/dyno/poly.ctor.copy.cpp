// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/builtin.hpp"
#include "caramel-poly/Trait.hpp"
#include "caramel-poly/Poly.hpp"

#include "awful.hpp"

#include <utility>

namespace /* anonymous */ {

TEST(DynoTest, CtorCopy) {
  // Make sure the copy constructor is not instantiated unless requested.
  {
    using NonCopyable = decltype(caramel::poly::require(caramel::poly::MoveConstructible{},
                                                caramel::poly::Destructible{}));
    caramel::poly::Poly<NonCopyable> a{awful::noncopyable{}};
    caramel::poly::Poly<NonCopyable> b{std::move(a)};
  }
}

} // anonymous namespace
