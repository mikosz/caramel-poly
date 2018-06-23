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
		makeConceptMapEntry(COMPILE_TIME_STRING("Method1"), Method1{})/*,
		makeConceptMapEntry(COMPILE_TIME_STRING("Method2"), Method2{}),
		makeConceptMapEntry(COMPILE_TIME_STRING("Method3"), Method3{})*/
		);

	constexpr auto method1Name = COMPILE_TIME_STRING("Method1");
	using Method1NameT = decltype(method1Name);
	using Method1LambdaTypeEntry = decltype(conceptMap)::LambdaType<Method1NameT>;
	using Method1LambdaTypeEntryType = Method1LambdaTypeEntry::Type;
	using Method1LambdaTypeEntryTypeLabmda = Method1LambdaTypeEntryType::Lambda;
	static_assert(std::is_same_v<Method1LambdaTypeEntryTypeLabmda, Method1>);

	//constexpr auto method2 = conceptMap.get(COMPILE_TIME_STRING("Method2"));
	//static_assert(std::is_same_v<std::remove_const_t<decltype(method2)::Lambda>, Method2>);

	//constexpr auto method3 = conceptMap.get(COMPILE_TIME_STRING("Method3"));
	//static_assert(std::is_same_v<std::remove_const_t<decltype(method3)::Lambda>, Method3>);

	// This should not (and does not) compile:
	// constexpr auto method4 = conceptMap.get(COMPILE_TIME_STRING("Method4"));
}

} // anonymous namespace
