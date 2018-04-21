#include <gtest/gtest.h>

#include "caramel-poly/vtable/Map.hpp"

namespace /* anonymous */ {

struct Entry {
	const int key;
};

} // anonymous namespace

namespace caramel_poly::vtable {

template <>
struct MapEntryKey<Entry> {
	constexpr auto operator()(const Entry& entry) const {
		return entry.key;
	}
};

} // namespace caramel_poly::vtable

namespace /* anonymous */ {

using namespace caramel_poly::vtable;

TEST(ContainerTest, ReturnsStoredElements) {
	constexpr auto container = makeMap(Entry{ 42 }, Entry{ 84 });
	static_assert(container[42].key == 42);
	static_assert(container[84].key == 84);
}

} // anonymous namespace
