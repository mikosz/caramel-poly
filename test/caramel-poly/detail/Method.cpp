#include <gtest/gtest.h>

#include "caramel-poly/detail/Method.hpp"
#include "caramel-poly/vtable/Concept.hpp"
#include "caramel-poly/compile-time/String.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct S {
};

TEST(ConceptTest, ReturnsStoredElements) {
	constexpr auto fooS = COMPILE_TIME_STRING("foo");
	constexpr auto barS = COMPILE_TIME_STRING("bar");
	constexpr auto bazS = COMPILE_TIME_STRING("baz");

	constexpr auto interfaceConcept = makeConcept(
		makeConceptEntry(fooS, MethodSignature<int (float)>{}),
		makeConceptEntry(barS, MethodSignature<const S& (S&&, float) const>{})
	);

	using FooSignature = decltype(interfaceConcept.methodSignature(fooS));
	static_assert(std::is_same_v<FooSignature, MethodSignature<int (float)>>);
	static_assert(std::is_same_v<FooSignature::MappingSignature, int (Object&, float)>);

	using BarSignature = decltype(interfaceConcept.methodSignature(barS));
	static_assert(std::is_same_v<BarSignature, MethodSignature<const S& (S&&, float) const>>);
	static_assert(std::is_same_v<BarSignature::MappingSignature, const S& (const Object&, S&&, float)>);

	// This should not (and does not) compile:
	// using BazSignature = decltype(interfaceConcept.methodSignature(bazS));
}

} // anonymous namespace
