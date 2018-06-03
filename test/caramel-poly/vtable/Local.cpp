#include <gtest/gtest.h>

#include "caramel-poly/compile-time/String.hpp"
#include "caramel-poly/vtable/MethodSignature.hpp"
#include "caramel-poly/vtable/Local.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct MethodReturns1 {
	int operator()() const {
		return 1;
	}
};

struct MethodMultipliesBy2 {
	int operator()(int value) const {
		return value * 2;
	}
};

TEST(StaticTest, InvokesAssignedMethods) {
	constexpr auto methodReturns1Name = COMPILE_TIME_STRING("MethodReturns1");
	constexpr auto methodMultipliesBy2Name = COMPILE_TIME_STRING("MethodMultipliesBy2");

	constexpr auto concept = makeConcept(
		makeConceptEntry(methodReturns1Name, MethodSignature<int ()>{}),
		makeConceptEntry(methodMultipliesBy2Name, MethodSignature<int (int)>{})
		);

	constexpr auto conceptMap = makeConceptMap(
		makeConceptMapEntry(methodReturns1Name, MethodReturns1{}),
		makeConceptMapEntry(methodMultipliesBy2Name, MethodMultipliesBy2{})
		);

	constexpr auto localVtable = makeLocal<void>(concept, conceptMap);

	EXPECT_EQ(localVtable.invoke<int>(methodReturns1Name), 1);
	EXPECT_EQ(localVtable.invoke<int>(methodMultipliesBy2Name, 21), 42);
}

} // anonymous namespace
