#include <gtest/gtest.h>

#include "caramel-poly/vtable/ConceptMap.hpp"
#include "caramel-poly/compile-time/String.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct S {};

constexpr auto method1Name = COMPILE_TIME_STRING("Method1");
constexpr auto method2Name = COMPILE_TIME_STRING("Method2");
constexpr auto method3Name = COMPILE_TIME_STRING("Method3");

struct Interface {};

struct Method1 {};
struct Method2 {};
struct Method3 {};

} // anonymous namespace

template <>
constexpr auto conceptMap<Interface, S> = makeConceptMap<S>(
	makeConceptMapEntry(method1Name, Method1{}),
	makeConceptMapEntry(method2Name, Method2{}),
	makeConceptMapEntry(method3Name, Method3{})
	);

namespace /* anonymous */ {

TEST(ConceptMapTest, ReturnsStoredElements) {
	using Method1LambdaTypeEntry = decltype(conceptMap<Interface, S>)::LambdaType<decltype(method1Name)>;
	static_assert(std::is_same_v<Method1LambdaTypeEntry, Method1>);

	using Method2LambdaTypeEntry = decltype(conceptMap<Interface, S>)::LambdaType<decltype(method2Name)>;
	static_assert(std::is_same_v<Method2LambdaTypeEntry, Method2>);

	using Method3LambdaTypeEntry = decltype(conceptMap<Interface, S>)::LambdaType<decltype(method3Name)>;
	static_assert(std::is_same_v<Method3LambdaTypeEntry, Method3>);

	// This should not (and does not) compile:
	//constexpr auto method4Name = COMPILE_TIME_STRING("Method4");
	//using Method4LambdaTypeEntry = typename decltype(conceptMap)::LambdaType<decltype(method4Name)>;
}

} // anonymous namespace
