#include <gtest/gtest.h>

#include "caramel-poly/compile-time/String.hpp"
#include "caramel-poly/Poly.hpp"

using namespace caramel_poly;

namespace /* anonymous */ {

constexpr auto methodReturns1Name = COMPILE_TIME_STRING("MethodReturns1");
constexpr auto methodMultipliesByIName = COMPILE_TIME_STRING("MethodMultipliesByI");

struct Interface : decltype(
	vtable::makeConcept(
		vtable::makeConceptEntry(methodReturns1Name, vtable::MethodSignature<int () const>{}),
		vtable::makeConceptEntry(methodMultipliesByIName, vtable::MethodSignature<int (int) const>{})
		)
	)
{
};

struct S {
	int i = 0;
};

} // anonymous namespace

constexpr auto lambada1 = [](const S&) { return 1; };
constexpr auto lambada2 = [](const S& s, int i) { return s.i * i; };

template <>
const auto vtable::conceptMap<Interface, S> = vtable::makeConceptMap<S>(
		vtable::makeConceptMapEntry(
			methodReturns1Name,
			lambada1
		),
		vtable::makeConceptMapEntry(
			methodMultipliesByIName,
			lambada2
		)
	);

namespace /* anonymous */ {

TEST(StaticTest, InvokesAssignedMethods) {
	auto s = S{ 2 };
	const auto p = Poly<Interface>{ std::move(s) };

	EXPECT_EQ(p.invoke(methodReturns1Name), 1);
	EXPECT_EQ(p.invoke(methodMultipliesByIName, 21), 42);
}

} // anonymous namespace
