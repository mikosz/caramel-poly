#include <gtest/gtest.h>

#include "caramel-poly/test/ConstructionRegistry.hpp"
#include "caramel-poly/vtable/DefaultConstructibleLambda.hpp"
#include "caramel-poly/detail/Method.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;
using namespace caramel_poly::vtable;

struct S {
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
	}

	EXPECT_TRUE(registry.allDestructed());
}

} // anonymous namespace
