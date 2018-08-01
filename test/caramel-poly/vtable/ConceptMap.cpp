#include <gtest/gtest.h>

#include "caramel-poly/vtable/ConceptMap.hpp"
#include "caramel-poly/compile-time/String.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct S {};

struct Method1 {};
struct Method2 {};
struct Method3 {};

TEST(ConceptMapTest, ReturnsStoredElements) {
	constexpr auto conceptMap = makeConceptMap<S>(
		makeConceptMapEntry(COMPILE_TIME_STRING("Method1"), Method1{}),
		makeConceptMapEntry(COMPILE_TIME_STRING("Method2"), Method2{}),
		makeConceptMapEntry(COMPILE_TIME_STRING("Method3"), Method3{})
		);

	constexpr auto method1Name = COMPILE_TIME_STRING("Method1");
	using Method1LambdaTypeEntry = decltype(conceptMap)::LambdaType<decltype(method1Name)>;
	static_assert(std::is_same_v<Method1LambdaTypeEntry, Method1>);

	constexpr auto method2Name = COMPILE_TIME_STRING("Method2");
	using Method2LambdaTypeEntry = decltype(conceptMap)::LambdaType<decltype(method2Name)>;
	static_assert(std::is_same_v<Method2LambdaTypeEntry, Method2>);

	constexpr auto method3Name = COMPILE_TIME_STRING("Method3");
	using Method3LambdaTypeEntry = decltype(conceptMap)::LambdaType<decltype(method3Name)>;
	static_assert(std::is_same_v<Method3LambdaTypeEntry, Method3>);

	// This should not (and does not) compile:
	//constexpr auto method4Name = COMPILE_TIME_STRING("Method4");
	//using Method4LambdaTypeEntry = typename decltype(conceptMap)::LambdaType<decltype(method4Name)>;
}

} // anonymous namespace
