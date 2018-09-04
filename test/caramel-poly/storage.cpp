// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>
#include <gtest/gtest-typed-test.h>

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

struct ObjectInterface : decltype(caramel_poly::requires(
	caramel_poly::Storable{},
	caramel_poly::Destructible{},
	caramel_poly::CopyConstructible{}
	))
{
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

template <class T>
struct StorageTest : ::testing::Test {
};

using StorageTestTypes = ::testing::Types<
	SBOStorage<sizeof(void*)>,
	RemoteStorage,
	SharedRemoteStorage,
	LocalStorage<sizeof(test::ConstructionRegistry::Object)>
	>;
TYPED_TEST_CASE(StorageTest, StorageTestTypes);

TYPED_TEST(StorageTest, DestroysStoredObject) {
	auto registry = test::ConstructionRegistry();

	{
		auto s = TypeParam(test::ConstructionRegistry::Object(registry));

		const auto complete = completeConceptMap<ObjectInterface, test::ConstructionRegistry::Object>(
			conceptMap<ObjectInterface, test::ConstructionRegistry::Object>);
		using VTable = VTable<Local<Everything>>;
		auto vtable = VTable::Type<ObjectInterface>{complete};

		s.destruct(vtable);
	}

	EXPECT_TRUE(registry.allDestructed());
}

TYPED_TEST(StorageTest, MovesStoredObject) {
	auto registry = test::ConstructionRegistry();

	{
		auto original = test::ConstructionRegistry::Object(registry);
		auto s = TypeParam(std::move(original));

		const auto complete = completeConceptMap<ObjectInterface, test::ConstructionRegistry::Object>(
			conceptMap<ObjectInterface, test::ConstructionRegistry::Object>);
		using VTable = VTable<Local<Everything>>;
		auto vtable = VTable::Type<ObjectInterface>{complete};

		auto m = TypeParam(std::move(s), vtable);

		const auto& state = m.get<test::ConstructionRegistry::Object>()->state();
		EXPECT_TRUE(state.moveConstructed);
		EXPECT_EQ(state.original, &original);
	}
}

TYPED_TEST(StorageTest, CopiesStoredObject) {
	auto registry = test::ConstructionRegistry();

	{
		auto original = test::ConstructionRegistry::Object(registry);
		auto s = TypeParam(original);

		const auto complete = completeConceptMap<ObjectInterface, test::ConstructionRegistry::Object>(
			conceptMap<ObjectInterface, test::ConstructionRegistry::Object>);
		using VTable = VTable<Local<Everything>>;
		auto vtable = VTable::Type<ObjectInterface>{complete};

		auto m = TypeParam(s, vtable);

		const auto& state = m.get<test::ConstructionRegistry::Object>()->state();
		EXPECT_TRUE(state.copyConstructed);
		EXPECT_EQ(state.original, &original);
	}
}

} // anonymous namespace
