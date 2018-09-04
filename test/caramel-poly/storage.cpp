// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/vtable.hpp"
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

TEST(StorageTest, DestroysStoredObject) {
	using TestValue = SBOStorage<sizeof(void*)>;

	auto registry = test::ConstructionRegistry();

	{
		auto s = TestValue(test::ConstructionRegistry::Object(registry));

		const auto complete = completeConceptMap<Destructible, test::ConstructionRegistry::Object>(
			conceptMap<Destructible, test::ConstructionRegistry::Object>);
		using VTable = VTable<Local<Everything>>;
		auto vtable = VTable::Type<Destructible>{complete};

		s.destruct(vtable);
	}

	EXPECT_TRUE(registry.allDestructed());
}

TEST(StorageTest, MovesStoredObject) {
	using TestValue = SBOStorage<sizeof(void*)>;

	auto registry = test::ConstructionRegistry();

	{
		auto original = test::ConstructionRegistry::Object(registry);
		auto s = TestValue(std::move(original));

		const auto complete = completeConceptMap<MoveConstructible, test::ConstructionRegistry::Object>(
			conceptMap<MoveConstructible, test::ConstructionRegistry::Object>);
		using VTable = VTable<Local<Everything>>;
		auto vtable = VTable::Type<MoveConstructible>{complete};

		auto m = TestValue(std::move(s), vtable);

		const auto& state = m.get<test::ConstructionRegistry::Object>()->state();
		EXPECT_TRUE(state.moveConstructed);
		EXPECT_EQ(state.original, &original);
		EXPECT_EQ(state.immediateOriginal, s.get<test::ConstructionRegistry::Object>());
	}
}

TEST(StorageTest, CopiesStoredObject) {
	using TestValue = SBOStorage<sizeof(void*)>;

	auto registry = test::ConstructionRegistry();

	{
		auto original = test::ConstructionRegistry::Object(registry);
		auto s = TestValue(original);

		const auto complete = completeConceptMap<CopyConstructible, test::ConstructionRegistry::Object>(
			conceptMap<CopyConstructible, test::ConstructionRegistry::Object>);
		using VTable = VTable<Local<Everything>>;
		auto vtable = VTable::Type<CopyConstructible>{complete};

		auto m = TestValue(s, vtable);

		const auto& state = m.get<test::ConstructionRegistry::Object>()->state();
		EXPECT_TRUE(state.copyConstructed);
		EXPECT_EQ(state.original, &original);
		EXPECT_EQ(state.immediateOriginal, s.get<test::ConstructionRegistry::Object>());
	}
}

} // anonymous namespace
