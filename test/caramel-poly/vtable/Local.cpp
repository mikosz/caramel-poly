#include <gtest/gtest.h>

#include "caramel-poly/compile-time/String.hpp"
#include "caramel-poly/vtable/MethodSignature.hpp"
#include "caramel-poly/vtable/Local.hpp"

namespace caramel_poly::vtable {

// TEMP!
struct Object {
};

} // namespace caramel_poly::vtable

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct S {
};

int returns1(const Object& /*self*/) {
	return 1;
}

int multipliesBy2(const Object& /*self*/, int value) {
	return value * 2;
}

//struct MethodReturns1 {
//	int operator()() const {
//		return 1;
//	}
//};
//
//struct MethodMultipliesBy2 {
//	int operator()(int value) const {
//		return value * 2;
//	}
//};

TEST(StaticTest, InvokesAssignedMethods) {
	constexpr auto methodReturns1Name = COMPILE_TIME_STRING("MethodReturns1");
	//constexpr auto methodMultipliesBy2Name = COMPILE_TIME_STRING("MethodMultipliesBy2");

	constexpr auto concept = makeConcept(
		makeConceptEntry(methodReturns1Name, MethodSignature<int () const>{})/*,
		makeConceptEntry(methodMultipliesBy2Name, MethodSignature<int (int) const>{})*/
		);

	constexpr auto conceptMap = makeConceptMap(
		makeConceptMapEntry(methodReturns1Name, &returns1)/*,
		makeConceptMapEntry(methodMultipliesBy2Name, &multipliesBy2)*/
		);

	constexpr auto localVtable = makeLocal<S>(concept, conceptMap);

	EXPECT_EQ(localVtable.invoke<int>(methodReturns1Name, Object{}), 1);
	//EXPECT_EQ(localVtable.invoke<int>(methodMultipliesBy2Name, 21), 42);
}

} // anonymous namespace
