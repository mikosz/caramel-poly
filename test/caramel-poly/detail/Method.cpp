#include <gtest/gtest.h>

#include "caramel-poly/vtable/DefaultConstructibleLambda.hpp"
#include "caramel-poly/detail/Method.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::detail;
using namespace caramel_poly::vtable;

struct S {
};

TEST(ConceptTest, CallsStoredFunction) {
	const auto returns42 = [](const S&) { return 42; }
	const auto method = Method<int (const Object&)>{ DefaultConstructibleLambda<decltype(returns42), int (const S&)> };

	EXPECT_EQ(method.invoke(S{}), 42);
}

} // anonymous namespace
