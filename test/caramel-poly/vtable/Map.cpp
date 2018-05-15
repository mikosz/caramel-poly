#include <gtest/gtest.h>

#include "caramel-poly/vtable/Map.hpp"

namespace /* anonymous */ {

template <uint32_t crc>
struct Entry {

	uint32_t key = crc;

	constexpr Entry() :
		key(crc)
	{
	}

};

} // anonymous namespace

namespace caramel_poly::vtable {

template <uint32_t crc>
struct MapEntryKey<Entry<crc>> {
	constexpr auto operator()() const {
		return crc;
	}
};

} // namespace caramel_poly::vtable

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace caramel_poly::vtable;

TEST(MapTest, ReturnsStoredElements) {
	//constexpr auto name1 = const_string::ConstString("name 1");
	//constexpr auto name2 = const_string::ConstString("name 2");
	//constexpr auto map = makeMap(Entry<name1.crc32()>(), Entry<name2.crc32()>());
	//static_assert(map.get<uint32_t, name1.crc32()>().key == name1.crc32());
	//static_assert(map.get<uint32_t, name2.crc32()>().key == name2.crc32());
}

} // anonymous namespace
