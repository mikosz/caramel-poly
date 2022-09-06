// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/Trait.hpp"

namespace /* anonymous */ {

using namespace caramel::poly;

TEST(TraitTest, RequiresConstructsATrait) {
	const auto fooName = POLY_FUNCTION_LABEL("foo");
	const auto barName = POLY_FUNCTION_LABEL("bar");
	const auto bazName = POLY_FUNCTION_LABEL("baz");

	const auto parent = require(
		POLY_FUNCTION_LABEL("foo") = method<void (int)>
		);

	const auto trait = require(
		parent,
		// Should not compile with error stating that the trait redefines a parent's method
		//POLY_FUNCTION_LABEL("foo") = method<void (int)>,
		POLY_FUNCTION_LABEL("bar") = method<float () const>,
		// Should not compile with error stating that the trait has a duplicate method
		//POLY_FUNCTION_LABEL("bar") = method<void (int)>,
		POLY_FUNCTION_LABEL("baz") = method<std::string (double) &&>
		);

	static_assert(std::is_same_v<
		decltype(trait.getSignature(fooName))::Type,
		void (SelfPlaceholder&, int)
		>);
	static_assert(std::is_same_v<
		decltype(trait.getSignature(barName))::Type,
		float (const SelfPlaceholder&)
		>);
	static_assert(std::is_same_v<
		decltype(trait.getSignature(bazName))::Type,
		std::string (SelfPlaceholder&&, double)
		>);

	// Should not compile with error stating that function was not found
	//const auto bzzName = POLY_FUNCTION_LABEL("bzz");
	//trait.getSignature(bzzName);

	constexpr auto names = detail::clauseNames(trait);
	static_assert(std::is_same_v<
		std::decay_t<decltype(names)>,
		decltype(detail::makeConstexprList(fooName, barName, bazName))
		>);
}

} // anonymous namespace
