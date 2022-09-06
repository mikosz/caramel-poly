// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include "caramel-poly/builtin.hpp"
#include "caramel-poly/Trait.hpp"
#include "caramel-poly/TraitMap.hpp"

// This test makes sure that our builtin concepts properly report that types
// that do not model them do not, instead of failing because of lack of
// SFINAE-friendliness.

namespace /* anonymous */ {

struct non_default_constructible { non_default_constructible() = delete; };
struct non_move_constructible { non_move_constructible(non_move_constructible&&) = delete; };
struct non_copy_constructible { non_copy_constructible(non_copy_constructible const&) = delete; };
struct non_move_assignable { non_move_assignable& operator=(non_move_assignable&&) = delete; };
struct non_copy_assignable { non_copy_assignable& operator=(non_copy_assignable const&) = delete; };
struct non_equality_comparable { friend bool operator==(non_equality_comparable const&, non_equality_comparable const&) = delete; };
struct non_destructible { ~non_destructible() = delete; };

static_assert(!caramel::poly::models<caramel::poly::DefaultConstructible, non_default_constructible>, "");
static_assert(!caramel::poly::models<caramel::poly::MoveConstructible, non_move_constructible>, "");
static_assert(!caramel::poly::models<caramel::poly::CopyConstructible, non_copy_constructible>, "");
static_assert(caramel::poly::models<caramel::poly::MoveAssignable, non_move_assignable>, "");
static_assert(caramel::poly::models<caramel::poly::CopyAssignable, non_copy_assignable>, "");
static_assert(!caramel::poly::models<caramel::poly::EqualityComparable, non_equality_comparable>, "");
static_assert(!caramel::poly::models<caramel::poly::Destructible, non_destructible>, "");

} // anonymous namespace
