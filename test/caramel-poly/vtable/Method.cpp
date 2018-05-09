#include <gtest/gtest.h>

#include "caramel-poly/vtable/Method.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct Functor {
	int operator()(int arg) const {
		return arg;
	}
};

TEST(VtableTest, MethodsAreCallable) {
	constexpr auto fooName = const_string::ConstString("foo");
	constexpr auto fooMethod = Method<fooName.crc32(), Functor>();
	EXPECT_EQ(fooMethod.invoke(42), 42);
}

} // anonymous namespace
