// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Trait.hpp"
#include "caramel-poly/TraitMap.hpp"
#include "caramel-poly/Poly.hpp"

// This test makes sure that we can implicitly construct a `caramel::poly::poly` from
// anything that satisfies the trait in the poly. We also check that we can't
// construct from something that does not model the trait.

namespace /* anonymous */ {

constexpr auto f_NAME = POLY_FUNCTION_LABEL("f");
constexpr auto g_NAME = POLY_FUNCTION_LABEL("g");

struct Trait : decltype(caramel::poly::require(
	f_NAME = caramel::poly::function<int(caramel::poly::SelfPlaceholder&)>,
	g_NAME = caramel::poly::function<int(caramel::poly::SelfPlaceholder&)>
)) { };

struct Foo { };
struct Bar { };

} // anonymous namespace

template <class T>
auto const caramel::poly::conceptMap<Trait, T, std::enable_if_t<std::is_same_v<T, Foo>>> = caramel::poly::makeTraitMap(
	f_NAME = [](Foo&) { return 111; },
	g_NAME = [](Foo&) { return 888; }
);

template <class T>
auto const caramel::poly::conceptMap<Trait, T, std::enable_if_t<std::is_same_v<T, Bar>>> = caramel::poly::makeTraitMap(
	f_NAME = [](Foo&) { return 111; }
	// missing `g` to model the trait
);

namespace /* anonymous */ {

static_assert(std::is_convertible<Foo, caramel::poly::Poly<Trait>>{}, "");
static_assert(!std::is_convertible<Bar, caramel::poly::Poly<Trait>>{}, "");

void f(caramel::poly::Poly<Trait> poly) {
	EXPECT_EQ(poly.virtual_(f_NAME)(poly), 111);
	EXPECT_EQ(poly.virtual_(g_NAME)(poly), 888);
}

TEST(DynoTest, CtorImplicit) {
	{
		// Implicit move-constructor
		f(Foo{});
	}

	{
		// Implicit copy-constructor
		Foo foo;
		f(foo);
	}
}

} // anonymous namespace
