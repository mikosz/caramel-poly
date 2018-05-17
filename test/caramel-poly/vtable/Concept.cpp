#include <gtest/gtest.h>

#include "caramel-poly/vtable/Concept.hpp"
#include "caramel-poly/compile-time/String.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct S {
};

TEST(ConceptTest, ReturnsStoredElements) {
	constexpr auto fooS = COMPILE_TIME_STRING("foo");
	using InterfaceConcept = decltype(makeConcept(
		makeConceptEntry(decltype(fooS), Method<int (float) const>{})//,
		//ConceptEntry<decltype(COMPILE_TIME_STRING("bar")), const S& (S&&, float*)>
		));

	static_assert(std::is_same_v<InterfaceConcept::Signature<decltype(fooS)>, int (float) const>);
}

} // anonymous namespace
