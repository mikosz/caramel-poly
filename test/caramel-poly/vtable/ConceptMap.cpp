#include <gtest/gtest.h>

#include "caramel-poly/vtable/ConceptMap.hpp"
#include "caramel-poly/compile-time/String.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct Method1 {};
struct Method2 {};
struct Method3 {};

TEST(ConceptMapTest, ReturnsStoredElements) {
	constexpr auto conceptMap = makeConceptMap(
		makeConceptMapEntry(COMPILE_TIME_STRING("Method1"), Method1{}),
		makeConceptMapEntry(COMPILE_TIME_STRING("Method2"), Method2{}),
		makeConceptMapEntry(COMPILE_TIME_STRING("Method3"), Method3{})
		);

	constexpr auto method1 = conceptMap.get(COMPILE_TIME_STRING("Method1"));
	static_assert(std::is_same_v<std::remove_const_t<decltype(method1)>, Method1>);

	constexpr auto method2 = conceptMap.get(COMPILE_TIME_STRING("Method2"));
	static_assert(std::is_same_v<std::remove_const_t<decltype(method2)>, Method2>);

	constexpr auto method3 = conceptMap.get(COMPILE_TIME_STRING("Method3"));
	static_assert(std::is_same_v<std::remove_const_t<decltype(method3)>, Method3>);
}

} // anonymous namespace
