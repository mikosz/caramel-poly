#include <gtest/gtest.h>

#include "caramel-poly/test/ConstructionRegistry.hpp"
#include "caramel-poly/vtable/DefaultConstructibleLambda.hpp"
#include "caramel-poly/detail/Method.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;
using namespace caramel_poly::vtable;

struct S {
	enum {
		CONSTRUCTED_AT_CALL_SITE,
		DEFAULT_CONSTRUCTED,
		COPY_CONSTRUCTED,
		COPY_ASSIGNED,
		MOVE_CONSTRUCTED,
		MOVE_ASSIGNED,
	} type;

	S() :
		type(DEFAULT_CONSTRUCTED)
	{
	}

	S(const S&) :
		type(COPY_CONSTRUCTED)
	{
	}

	S& operator=(const S&) {
		type = COPY_ASSIGNED;
		return *this;
	}

	S(S&&) :
		type(MOVE_CONSTRUCTED)
	{
	}

	S& operator=(S&&) {
		type = MOVE_ASSIGNED;
		return *this;
	}
};

TEST(ConceptTest, CallsStoredFunction) {
	const auto returns42 = [](const S&) { return 42; };
	const auto multipliesBy2 = [](const S&, int i) { return i * 2; };
	const auto returns42DCL = DefaultConstructibleLambda<decltype(returns42), int(const S&)>{};
	const auto multipliesBy2DCL = DefaultConstructibleLambda<decltype(multipliesBy2), int(const S&, int)>{};
	
	const auto returns42Method = Method<int (const Object&)>(returns42DCL);
	EXPECT_EQ(returns42Method.invoke(S{}), 42);

	const auto multipliesBy2Method = Method<int (const Object&, int)>(multipliesBy2DCL);
	EXPECT_EQ(multipliesBy2Method.invoke(S{}, 21), 42);
}

TEST(ConceptTest, SelfIsPassedAsExpected) {
	using RegistryObject = test::ConstructionRegistry::Object;
	auto registry = test::ConstructionRegistry{};

	{
		auto object = test::ConstructionRegistry::Object{ registry };

		{
			const auto reference = [](RegistryObject& self, const RegistryObject* original) {
					const auto& selfState = self.state();
					EXPECT_TRUE(selfState.constructed);
					EXPECT_EQ(&self, original);
				};
			const auto referenceDCL =
				DefaultConstructibleLambda<decltype(reference), void (RegistryObject&, const RegistryObject*)>{};
			const auto referenceMethod = Method<void (Object&, const RegistryObject*)>(referenceDCL);
	
			referenceMethod.invoke(object, &object);
		}

		{
			const auto constReference = [](const RegistryObject& self, const RegistryObject* original) {
					const auto& selfState = self.state();
					EXPECT_TRUE(selfState.constructed);
					EXPECT_EQ(&self, original);
				};
			const auto constReferenceDCL =
				DefaultConstructibleLambda<decltype(constReference), void (const RegistryObject&, const RegistryObject*)>{};
			const auto constReferenceMethod = Method<void (const Object&, const RegistryObject*)>(constReferenceDCL);

			constReferenceMethod.invoke(object, &object);
		}

		{
			const auto pointer = [](RegistryObject* self, const RegistryObject* original) {
					const auto& selfState = self->state();
					EXPECT_TRUE(selfState.constructed);
					EXPECT_EQ(self, original);
				};
			const auto pointerDCL =
				DefaultConstructibleLambda<decltype(pointer), void (RegistryObject*, const RegistryObject*)>{};
			const auto pointerMethod = Method<void (Object*, const RegistryObject*)>(pointerDCL);
	
			//pointerMethod.invoke(&object, &object);
		}

		//const auto pointerToConst = [](const RegistryObject*) {};
		//const auto pointerToConstDCL =
		//	DefaultConstructibleLambda<decltype(pointerToConst), void (const RegistryObject*)>{};
		//const auto pointerToConstMethod = Method<void (const Object*)>(pointerToConstDCL);
	}
}

} // anonymous namespace
