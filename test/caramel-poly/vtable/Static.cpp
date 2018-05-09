#include <gtest/gtest.h>

#include "caramel-poly/vtable/Static.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

struct Foo {
	int operator()() const {
		return 42;
	}
};

struct Bar {
	float operator()() const {
		return 3.14f;
	}
};

TEST(StaticTest, ReturnsAssignedMethods) {
	//constexpr auto foo = const_string::ConstString("foo");
	//constexpr auto bar = const_string::ConstString("bar");

	//constexpr auto staticVTable = makeStatic(
	//	Method<foo.crc32(), Foo>{},
	//	Method<bar.crc32(), Bar>{}
	//	);

	//EXPECT_EQ(staticVTable.invoke<int>(const_string::ConstString("foo")).invoke(), 42);
	//EXPECT_FLOAT_EQ(staticVTable.invoke<float>(const_string::ConstString("bar")).invoke(), 3.14f);
}

} // anonymous namespace
