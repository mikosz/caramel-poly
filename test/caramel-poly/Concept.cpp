// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/Concept.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

TEST(ConceptTest, RequiresConstructsAConcept) {
	const auto fooName = POLY_FUNCTION_LABEL("foo");
	const auto barName = POLY_FUNCTION_LABEL("bar");
	const auto bazName = POLY_FUNCTION_LABEL("baz");

	const auto parent = requires(
		POLY_FUNCTION_LABEL("foo") = method<void (int)>
		);

	const auto concept = requires(
		parent,
		// Should not compile with error stating that the concept redefines a parent's method
		//POLY_FUNCTION_LABEL("foo") = method<void (int)>,
		POLY_FUNCTION_LABEL("bar") = method<float () const>,
		// Should not compile with error stating that the concept has a duplicate method
		//POLY_FUNCTION_LABEL("bar") = method<void (int)>,
		POLY_FUNCTION_LABEL("baz") = method<std::string (double) &&>
		);

	static_assert(std::is_same_v<
		decltype(concept.getSignature(fooName))::Type,
		void (SelfPlaceholder&, int)
		>);
	static_assert(std::is_same_v<
		decltype(concept.getSignature(barName))::Type,
		float (const SelfPlaceholder&)
		>);
	static_assert(std::is_same_v<
		decltype(concept.getSignature(bazName))::Type,
		std::string (SelfPlaceholder&&, double)
		>);

	// Should not compile with error stating that function was not found
	//const auto bzzName = POLY_FUNCTION_LABEL("bzz");
	//concept.getSignature(bzzName);

	constexpr auto names = detail::clauseNames(concept);
	static_assert(std::is_same_v<
		std::decay_t<decltype(names)>,
		decltype(detail::makeConstexprList(fooName, barName, bazName))
		>);
}

} // anonymous namespace
