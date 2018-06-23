#include <gtest/gtest.h>

#include "caramel-poly/compile-time/String.hpp"
#include "caramel-poly/vtable/MethodSignature.hpp"
#include "caramel-poly/vtable/Local.hpp"
#include "caramel-poly/vtable/DefaultConstructibleLambda.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct S {
	int i = 0;
};

TEST(StaticTest, InvokesAssignedMethods) {
	constexpr auto methodReturns1Name = COMPILE_TIME_STRING("MethodReturns1");
	constexpr auto methodMultipliesByIName = COMPILE_TIME_STRING("MethodMultipliesByI");

	constexpr auto concept = makeConcept(
		makeConceptEntry(methodReturns1Name, MethodSignature<int () const>{}),
		makeConceptEntry(methodMultipliesByIName, MethodSignature<int (int) const>{})
		);

	constexpr auto conceptMap = makeConceptMap<S>(
		makeConceptMapEntry(
			methodReturns1Name,
			[](const S&) { return 1; }
			),
		makeConceptMapEntry(
			methodMultipliesByIName,
			[](const S& s, int i) { return s.i * i; }
			)
		);

	constexpr auto localVtable = makeLocal(concept, conceptMap);

	EXPECT_EQ(localVtable.invoke(methodReturns1Name, S{}), 1);
	EXPECT_EQ(localVtable.invoke(methodMultipliesByIName, S{ 2 }, 21), 42);
}

} // anonymous namespace
