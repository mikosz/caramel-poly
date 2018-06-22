#include <gtest/gtest.h>

#include "caramel-poly/compile-time/String.hpp"
#include "caramel-poly/vtable/MethodSignature.hpp"
#include "caramel-poly/vtable/Local.hpp"
#include "caramel-poly/vtable/DefaultConstructibleLambda.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct S {
};

TEST(StaticTest, InvokesAssignedMethods) {
	constexpr auto methodReturns1Name = COMPILE_TIME_STRING("MethodReturns1");
	//constexpr auto methodMultipliesBy2Name = COMPILE_TIME_STRING("MethodMultipliesBy2");

	constexpr auto concept = makeConcept(
		makeConceptEntry(methodReturns1Name, MethodSignature<int () const>{})/*,
		makeConceptEntry(methodMultipliesBy2Name, MethodSignature<int (int) const>{})*/
		);

	auto returns1Lambda = [](const S&) { return 1; };

	constexpr auto conceptMap = makeConceptMap(
		makeConceptMapEntry(
			methodReturns1Name,
			DefaultConstructibleLambda<decltype(returns1Lambda), int (const S&)>{}
			)/*,
		makeConceptMapEntry(methodMultipliesBy2Name, &multipliesBy2)*/
		);

	constexpr auto localVtable = makeLocal(concept, conceptMap);

	EXPECT_EQ(localVtable.invoke(methodReturns1Name, S{}), 1);
	//EXPECT_EQ(localVtable.invoke<int>(methodMultipliesBy2Name, 21), 42);
}

} // anonymous namespace
