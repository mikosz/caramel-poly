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

constexpr auto methodReturns1Name = COMPILE_TIME_STRING("MethodReturns1");
constexpr auto methodMultipliesByIName = COMPILE_TIME_STRING("MethodMultipliesByI");

struct Interface : decltype(
	makeConcept(
		makeConceptEntry(methodReturns1Name, MethodSignature<int () const>{}),
		makeConceptEntry(methodMultipliesByIName, MethodSignature<int (int) const>{})
		)
	)
{
};

} // anonymous namespace

const auto returns1 = [](const S&) { return 1; };
const auto multipliesByI = [](const S& s, int i) { return s.i * i; };

template<>
const auto vtable::conceptMap<Interface, S> = makeConceptMap<S>(
	makeConceptMapEntry(
		methodReturns1Name,
		returns1
		),
	makeConceptMapEntry(
		methodMultipliesByIName,
		multipliesByI
		)
	);

namespace /* anonymous */ {

TEST(StaticTest, InvokesAssignedMethods) {
	const auto localVtable = Local<Interface>(vtable::conceptMap<Interface, S>);

	EXPECT_EQ(localVtable.invoke(methodReturns1Name, S{}), 1);
	EXPECT_EQ(localVtable.invoke(methodMultipliesByIName, S{ 2 }, 21), 42);
}

} // anonymous namespace
