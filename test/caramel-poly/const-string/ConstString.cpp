#include <gtest/gtest.h>

#include "caramel-poly/const-string/ConstString.hpp"

namespace /* anonymous */ {

using namespace caramel_poly::const_string;

TEST(ConstStringTest, ConstStringsHaveLength) {
	constexpr auto cs = ConstString("123456");
	static_assert(cs.length() == 6);
}

TEST(ConstStringTest, ConstStringCharsAreReadable) {
	constexpr auto cs = ConstString("01234");
	static_assert(cs[2] == '2');
}

TEST(ConstStringTest, ConstStringsAreEqualityTestable) {
	constexpr auto cs = ConstString("01234");
	constexpr auto same = ConstString("01234");
	constexpr auto different = ConstString("01_34");
	static_assert(cs == same);
	static_assert(cs != different);
}

} // anonymous namespace
