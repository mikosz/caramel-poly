// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Concept.hpp"
#include "caramel-poly/ConceptMap.hpp"
#include "caramel-poly/Poly.hpp"

// This test makes sure that we can implicitly construct a `caramel::poly::poly` from
// anything that satisfies the concept in the poly. We also check that we can't
// construct from something that does not model the concept.

namespace /* anonymous */ {

constexpr auto f_NAME = POLY_FUNCTION_LABEL("f");
constexpr auto g_NAME = POLY_FUNCTION_LABEL("g");

struct Concept : decltype(caramel::poly::requires(
	f_NAME = caramel::poly::function<int(caramel::poly::SelfPlaceholder&)>,
	g_NAME = caramel::poly::function<int(caramel::poly::SelfPlaceholder&)>
)) { };

struct Foo { };
struct Bar { };

} // anonymous namespace

template <class T>
auto const caramel::poly::conceptMap<Concept, T, std::enable_if_t<std::is_same_v<T, Foo>>> = caramel::poly::makeConceptMap(
	f_NAME = [](Foo&) { return 111; },
	g_NAME = [](Foo&) { return 888; }
);

template <class T>
auto const caramel::poly::conceptMap<Concept, T, std::enable_if_t<std::is_same_v<T, Bar>>> = caramel::poly::makeConceptMap(
	f_NAME = [](Foo&) { return 111; }
	// missing `g` to model the concept
);

namespace /* anonymous */ {

static_assert(std::is_convertible<Foo, caramel::poly::Poly<Concept>>{}, "");
static_assert(!std::is_convertible<Bar, caramel::poly::Poly<Concept>>{}, "");

void f(caramel::poly::Poly<Concept> poly) {
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
