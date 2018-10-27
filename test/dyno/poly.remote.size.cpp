// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include "caramel-poly/Concept.hpp"
#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

// This test makes sure that the size of a `caramel_poly::poly` with a remote storage
// policy and a remote vtable is the size of 2 pointers. There was once a bug
// where the size was more than that.

constexpr auto f_NAME = POLY_FUNCTION_LABEL("f");

struct Concept : decltype(caramel_poly::requires(
  f_NAME = caramel_poly::function<void (caramel_poly::SelfPlaceholder&)>
)) { };

using Storage = caramel_poly::RemoteStorage<>;
using VTable = caramel_poly::VTable<caramel_poly::Remote<caramel_poly::Everything>>;
using Poly = caramel_poly::Poly<Concept, Storage, VTable>;
static_assert(sizeof(Poly) == 2 * sizeof(void*));

} // anonymous namespace
