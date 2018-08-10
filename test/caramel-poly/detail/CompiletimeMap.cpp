// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/detail/CompileTimeMap.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;

template <int i>
struct S {
};

TEST(CompileTimeMapTest, ContainsReturnsWhetherTheMapContainsAnEntry) {
	using Map = CompileTimeMap<
		CompileTimeMapEntry<S<0>, int>,
		CompileTimeMapEntry<S<1>, char>,
		CompileTimeMapEntry<S<2>, double>
		>;

	static_assert(Map::contains<S<0>>());
	static_assert(Map::contains<S<1>>());
	static_assert(Map::contains<S<2>>());
	static_assert(!Map::contains<S<3>>());
}

TEST(CompileTimeMapTest, HasDuplicatesReturnsTrueIfMapContainsDuplicates) {
	using MapNoDuplicates = CompileTimeMap<
		CompileTimeMapEntry<S<0>, int>,
		CompileTimeMapEntry<S<1>, char>,
		CompileTimeMapEntry<S<2>, double>
		>;

	using MapFrontDuplicate = CompileTimeMap<
		CompileTimeMapEntry<S<0>, int>,
		CompileTimeMapEntry<S<1>, char>,
		CompileTimeMapEntry<S<0>, double>
		>;

	using MapMiddleDuplicate = CompileTimeMap<
		CompileTimeMapEntry<S<0>, int>,
		CompileTimeMapEntry<S<1>, char>,
		CompileTimeMapEntry<S<1>, double>
		>;

	static_assert(!MapNoDuplicates::hasDuplicates());
	static_assert(MapFrontDuplicate::hasDuplicates());
	static_assert(MapMiddleDuplicate::hasDuplicates());
}

} // anonymous namespace
