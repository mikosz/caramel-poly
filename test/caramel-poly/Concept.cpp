// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/Concept.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

TEST(ConceptTest, RequiresConstructsAConcept) {
	const auto fooName = METHOD_NAME("foo");
	const auto barName = METHOD_NAME("bar");
	const auto bazName = METHOD_NAME("baz");

	const auto parent = requires(
		METHOD_NAME("foo") = method<void (int)>
		);

	const auto concept = requires(
		parent,
		// Should not compile with error stating that the concept redefines a parent's method
		//METHOD_NAME("foo") = method<void (int)>,
		METHOD_NAME("bar") = method<float () const>,
		// Should not compile with error stating that the concept has a duplicate method
		//METHOD_NAME("bar") = method<void (int)>,
		METHOD_NAME("baz") = method<std::string (double) &&>
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
	//const auto bzzName = METHOD_NAME("bzz");
	//concept.getSignature(bzzName);
}

} // anonymous namespace
