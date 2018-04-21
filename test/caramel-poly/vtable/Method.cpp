#include <gtest/gtest.h>

#include "caramel-poly/vtable/Method.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

int foo(int arg) {
	return arg;
}

struct Functor {
	float operator()(float arg) const {
		return arg;
	}
};

TEST(VtableTest, MethodsHaveConstexprNames) {
	constexpr auto method = createMethod(const_string::ConstString("foo"), foo);
	static_assert(method.name() == const_string::ConstString("foo"));
}

TEST(VtableTest, MethodsAreCallable) {
	constexpr auto fooMethod = createMethod(const_string::ConstString("foo"), foo);
	EXPECT_EQ(fooMethod.invoke(42), 42);

	constexpr auto functorMethod = createMethod(const_string::ConstString("functor"), Functor());
	EXPECT_FLOAT_EQ(functorMethod.invoke(3.14f), 3.14f);

	constexpr auto lambdaMethod = createMethod(const_string::ConstString("lambda"), []() { return 123; });
	EXPECT_EQ(lambdaMethod.invoke(), 123);
}

} // anonymous namespace
