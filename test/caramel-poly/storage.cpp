// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>
#include <gtest/gtest-typed-test.h>

#include <type_traits>

#include "caramel-poly/vtable.hpp"
#include "caramel-poly/storage.hpp"
#include "../ConstructionRegistry.hpp"

namespace /* anonymous */ {

using namespace caramel::poly;

template <size_t PTRS>
struct S {
	void* p[PTRS];
};

struct ObjectInterface : decltype(caramel::poly::requires(
	caramel::poly::Storable{},
	caramel::poly::Destructible{},
	caramel::poly::CopyConstructible{},
	caramel::poly::MoveConstructible{}
	))
{
};

TEST(SBOStorageTest, StoresFittingDataInternally) {
	auto smallStorage = SBOStorage<sizeof(void*)>(S<1>{});
	auto* firstByte = reinterpret_cast<char*>(smallStorage.template get<S<1>>());
	EXPECT_GE(firstByte, reinterpret_cast<char*>(&smallStorage));
	EXPECT_LT(firstByte, reinterpret_cast<char*>(&smallStorage) + sizeof(smallStorage));
}

TEST(SBOStorageTest, StoresNonFittingDataExternally) {
	auto smallStorage = SBOStorage<sizeof(void*)>(S<2>{});
	auto* firstByte = reinterpret_cast<char*>(smallStorage.template get<S<2>>());
	EXPECT_TRUE(
		firstByte < reinterpret_cast<char*>(&smallStorage) ||
		firstByte >= reinterpret_cast<char*>(&smallStorage) + sizeof(smallStorage)
		);
}

template <class T>
struct AllStorageTest : ::testing::Test {};
template <class T>
struct RemoteStorageTest : ::testing::Test {};
template <class T>
struct LocalStorageTest : ::testing::Test {};
template <class T>
struct OwningStorageTest : ::testing::Test {};
template <class T>
struct NonOwningStorageTest : ::testing::Test {};

template <class StorageTypeT, class ObjectTypeT>
struct StorageScenario {
	using StorageType = StorageTypeT;
	using ObjectType = ObjectTypeT;
};

using SmallObject = test::ConstructionRegistry::Object;

template <size_t BYTES>
struct BigObject : test::ConstructionRegistry::Object {
	char _[BYTES - sizeof(test::ConstructionRegistry::Object)];
	using test::ConstructionRegistry::Object::Object;
};

using StorageScenarioSBOFitting = StorageScenario<SBOStorage<sizeof(SmallObject)>, SmallObject>;
using StorageScenarioSBONonFitting = StorageScenario<SBOStorage<sizeof(SmallObject)>, BigObject<128>>;
using StorageScenarioRemote = StorageScenario<RemoteStorage<>, SmallObject>;
using StorageScenarioSharedRemote = StorageScenario<SharedRemoteStorage<>, SmallObject>;
using StorageScenarioLocal = StorageScenario<LocalStorage<sizeof(SmallObject)>, SmallObject>;
using StorageScenarioNonOwning = StorageScenario<NonOwningStorage, SmallObject>;

using AllStorageTestTypes = ::testing::Types<
	StorageScenarioSBOFitting,
	StorageScenarioSBONonFitting,
	StorageScenarioRemote,
	StorageScenarioSharedRemote,
	StorageScenarioLocal,
	StorageScenarioNonOwning
	>;
TYPED_TEST_CASE(AllStorageTest, AllStorageTestTypes);

using RemoteStorageTestTypes = ::testing::Types<
	StorageScenarioSBONonFitting,
	StorageScenarioRemote,
	StorageScenarioSharedRemote,
	StorageScenarioNonOwning
	>;
TYPED_TEST_CASE(RemoteStorageTest, RemoteStorageTestTypes);

using LocalStorageTestTypes = ::testing::Types<
	StorageScenarioSBOFitting,
	StorageScenarioLocal
	>;
TYPED_TEST_CASE(LocalStorageTest, LocalStorageTestTypes);

using OwningStorageTestTypes = ::testing::Types<
	StorageScenarioSBOFitting,
	StorageScenarioSBONonFitting,
	StorageScenarioRemote,
	StorageScenarioSharedRemote,
	StorageScenarioLocal
>;
TYPED_TEST_CASE(OwningStorageTest, OwningStorageTestTypes);

using NonOwningStorageTestTypes = ::testing::Types<
	StorageScenarioNonOwning
>;
TYPED_TEST_CASE(NonOwningStorageTest, NonOwningStorageTestTypes);

TYPED_TEST(AllStorageTest, HasStorageInfoForObject) {
	using Storage = typename TypeParam::StorageType;
	using Object = typename TypeParam::ObjectType;

	auto registry = test::ConstructionRegistry();

	auto original = Object(registry);
	Storage s{original};

	const auto complete = completeConceptMap<ObjectInterface, Object>(conceptMap<ObjectInterface, Object>);
	using VTable = VTable<Local<Everything>>;
	auto vtable = VTable::Type<ObjectInterface>{complete};

	const auto storageInfo = vtable[STORAGE_INFO_LABEL]();

	EXPECT_EQ(storageInfo.size, sizeof(Object));
	EXPECT_EQ(storageInfo.alignment, alignof(Object));
}

TYPED_TEST(AllStorageTest, DestroysStoredObject) {
	using Storage = typename TypeParam::StorageType;
	using Object = typename TypeParam::ObjectType;

	auto registry = test::ConstructionRegistry();

	{
		auto original = Object(registry);
		Storage s{original};

		const auto complete = completeConceptMap<ObjectInterface, Object>(
			conceptMap<ObjectInterface, Object>);
		using VTable = VTable<Local<Everything>>;
		auto vtable = VTable::Type<ObjectInterface>{complete};

		s.destruct(vtable);
	}

	EXPECT_TRUE(registry.allDestructed());
}

TYPED_TEST(LocalStorageTest, MovesStoredObject) {
	using Storage = typename TypeParam::StorageType;
	using Object = typename TypeParam::ObjectType;

	auto registry = test::ConstructionRegistry();

	auto original = Object(registry);
	Storage s{original};

	const auto complete = completeConceptMap<ObjectInterface, Object>(
		conceptMap<ObjectInterface, Object>);
	using VTable = VTable<Local<Everything>>;
	auto vtable = VTable::Type<ObjectInterface>{complete};

	auto m = Storage(std::move(s), vtable);

	const auto& state = m.template get<Object>()->state();
	EXPECT_TRUE(state.moveConstructed);
	EXPECT_EQ(state.original, &original);
}

TYPED_TEST(RemoteStorageTest, MovesPointerToStoredObject) {
	using Storage = typename TypeParam::StorageType;
	using Object = typename TypeParam::ObjectType;

	auto registry = test::ConstructionRegistry();

	auto original = Object(registry);
	Storage s{original};
	auto* storedData = s.template get<Object>();

	const auto complete = completeConceptMap<ObjectInterface, Object>(
		conceptMap<ObjectInterface, Object>);
	using VTable = VTable<Local<Everything>>;
	auto vtable = VTable::Type<ObjectInterface>{complete};

	auto m = Storage(std::move(s), vtable);

	if constexpr (!std::is_same_v<Storage, NonOwningStorage>) {
		EXPECT_EQ(s.template get<Object>(), static_cast<Object*>(nullptr));
	}
	EXPECT_EQ(m.template get<Object>(), storedData);
}

TYPED_TEST(OwningStorageTest, CopiesStoredObject) {
	using Storage = typename TypeParam::StorageType;
	using Object = typename TypeParam::ObjectType;

	auto registry = test::ConstructionRegistry();

	auto original = Object(registry);
	auto s = Storage(original);

	const auto complete = completeConceptMap<ObjectInterface, Object>(
		conceptMap<ObjectInterface, Object>);
	using VTable = VTable<Local<Everything>>;
	auto vtable = VTable::Type<ObjectInterface>{complete};

	auto c = Storage(s, vtable);

	const auto& state = c.template get<Object>()->state();
	EXPECT_TRUE(state.copyConstructed);
	EXPECT_EQ(state.original, &original);
}

TYPED_TEST(NonOwningStorageTest, CopiesPointerToStoredObject) {
	using Storage = typename TypeParam::StorageType;
	using Object = typename TypeParam::ObjectType;

	auto registry = test::ConstructionRegistry();

	auto original = Object(registry);
	auto s = Storage(original);
	auto* storedData = s.template get<Object>();

	const auto complete = completeConceptMap<ObjectInterface, Object>(
		conceptMap<ObjectInterface, Object>);
	using VTable = VTable<Local<Everything>>;
	auto vtable = VTable::Type<ObjectInterface>{complete};

	auto c = Storage(s, vtable);

	EXPECT_EQ(s.template get<Object>(), storedData);
	EXPECT_EQ(c.template get<Object>(), storedData);
}

} // anonymous namespace
