// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/storage.hpp"
#include "../ConstructionRegistry.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

template <size_t PTRS>
struct S {
	void* p[PTRS];
};

TEST(SBOStorageTest, StoresFittingDataInternally) {
	auto smallStorage = SBOStorage<sizeof(void*)>(S<1>{});
	auto* firstByte = reinterpret_cast<char*>(smallStorage.get<S<1>>());
	EXPECT_GE(firstByte, reinterpret_cast<char*>(&smallStorage));
	EXPECT_LT(firstByte, reinterpret_cast<char*>(&smallStorage) + sizeof(smallStorage));
}

TEST(SBOStorageTest, StoresNonFittingDataExternally) {
	auto smallStorage = SBOStorage<sizeof(void*)>(S<2>{});
	auto* firstByte = reinterpret_cast<char*>(smallStorage.get<S<2>>());
	EXPECT_TRUE(
		firstByte < reinterpret_cast<char*>(&smallStorage) ||
		firstByte >= reinterpret_cast<char*>(&smallStorage) + sizeof(smallStorage)
		);
}

} // anonymous namespace
