// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/Concept.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

TEST(ConceptTest, RequiresConstructsAConcept) {
	const auto concept = requires(
		detail::makeConstexprPair(CONSTEXPR_STRING("foo"), method<void ()>),
		detail::makeConstexprPair(CONSTEXPR_STRING("bar"), method<void ()>)
		);
	switching bar to foo doesn't fail compilation, so something's wrong, need tests for redefine and has duplicates
}

} // anonymous namespace
