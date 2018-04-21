#include <gtest/gtest.h>

#include "caramel-poly/vtable/Container.hpp"

namespace /* anonymous */ {

using namespace caramel_poly::vtable;

struct Entry {
	const int key;
};

TEST(ContainerTest, ReturnsStoredElements) {
	constexpr auto e1 = Entry{ 42 };
	constexpr auto e2 = Entry{ 84 };
	constexpr auto container = makeMap(e1, e2);
	static_assert(container[42].key == 42);
	static_assert(container[84].key == 84);
}

} // anonymous namespace
