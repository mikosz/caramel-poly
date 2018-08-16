// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/Concept.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

TEST(ConceptTest, RequiresConstructsAConcept) {
	const auto fooName = CONSTEXPR_STRING("foo");
	const auto barName = CONSTEXPR_STRING("bar");
	const auto bazName = CONSTEXPR_STRING("baz");

	const auto parent = requires(
		detail::makeConstexprPair(fooName, method<void (int)>)
		);

	const auto concept = requires(
		parent,
		detail::makeConstexprPair(barName, method<float () const>),
		detail::makeConstexprPair(bazName, method<std::string (double) &&>)
		);

	static_assert(std::is_same_v<
		typename decltype(concept.getSignature(fooName))::Signature,
		void (int)
		>);
	static_assert(std::is_same_v<
		typename decltype(concept.getSignature(barName))::Signature,
		float () const
		>);
	static_assert(std::is_same_v<
		typename decltype(concept.getSignature(bazName))::Signature,
		std::string (double) &&
		>);

	// Does not and should not compile
	//const auto bzzName = CONSTEXPR_STRING("bzz");
	//concept.getSignature(bzzName);
}

} // anonymous namespace
