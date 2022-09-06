// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include "caramel-poly/Trait.hpp"
#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

// This test makes sure that the size of a `caramel::poly::poly` with a remote storage
// policy and a remote vtable is the size of 2 pointers. There was once a bug
// where the size was more than that.

constexpr auto f_NAME = POLY_FUNCTION_LABEL("f");

struct Trait : decltype(caramel::poly::require(
  f_NAME = caramel::poly::function<void (caramel::poly::SelfPlaceholder&)>
)) { };

using Storage = caramel::poly::RemoteStorage<>;
using VTable = caramel::poly::VTable<caramel::poly::Remote<caramel::poly::Everything>>;
using Poly = caramel::poly::Poly<Trait, Storage, VTable>;
static_assert(sizeof(Poly) == 2 * sizeof(void*));

} // anonymous namespace
