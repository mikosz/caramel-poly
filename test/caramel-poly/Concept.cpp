// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/Concept.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

TEST(ConceptTest, RequiresConstructsAConcept) {
	const auto parent = requires(
		detail::makeConstexprPair(CONSTEXPR_STRING("foo"), method<void (int)>)
		);

	const auto concept = requires(
		parent,
		detail::makeConstexprPair(CONSTEXPR_STRING("bar"), method<void ()>),
		detail::makeConstexprPair(CONSTEXPR_STRING("baz"), method<void (int)>)
		);
}

} // anonymous namespace
