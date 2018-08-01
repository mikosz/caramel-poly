#include <gtest/gtest.h>

#include "caramel-poly/compile-time/String.hpp"
#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

struct Interface;

struct S {
	int i = 0;
};

constexpr auto methodReturns1Name = COMPILE_TIME_STRING("MethodReturns1");
constexpr auto methodMultipliesByIName = COMPILE_TIME_STRING("MethodMultipliesByI");

template <>
const auto caramel_poly::conceptMap<Interface, S> = makeConceptMap<S>(
		makeConceptMapEntry(
			methodReturns1Name,
			[](const S&) { return 1; }
		),
		makeConceptMapEntry(
			methodMultipliesByIName,
			[](const S& s, int i) { return s.i * i; }
		)
	);

TEST(StaticTest, InvokesAssignedMethods) {
	constexpr auto concept = makeConcept(
		makeConceptEntry(methodReturns1Name, MethodSignature<int () const>{}),
		makeConceptEntry(methodMultipliesByIName, MethodSignature<int (int) const>{})
		);

	constexpr auto localVtable = makeLocal(concept, conceptMap);

	auto s = S{ 2 };
	const auto p = Poly<S>{ std::move(s) };

	EXPECT_EQ(p.invoke(methodReturns1Name), 1);
	EXPECT_EQ(p.invoke(methodMultipliesByIName, 21), 42);
}

} // anonymous namespace
