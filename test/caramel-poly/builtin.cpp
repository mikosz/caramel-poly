// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <string>

#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

using namespace caramel::poly;

struct Empty {
};

TEST(PolyTest, TypeIdAccessible) {
	auto sp = Poly<TypeId>{Empty{}};

	const auto storageInfo = sp.virtual_(STORAGE_INFO_LABEL)();
	EXPECT_EQ(storageInfo.size, sizeof(Empty));
	EXPECT_EQ(storageInfo.alignment, alignof(Empty));

	const auto& reportedTypeid = sp.virtual_(TYPEID_LABEL)();
	const auto& expectedTypeid = typeid(Empty);
	EXPECT_EQ(reportedTypeid, expectedTypeid);
}

} // anonymous namespace
