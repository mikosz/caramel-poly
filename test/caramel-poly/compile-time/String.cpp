#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "caramel-poly/compile-time/String.hpp"

namespace /* anonymous */ {

using namespace std::string_literals;
using namespace caramel_poly::compile_time;

TEST(StringTest, HasLength) {
	constexpr auto cs = COMPILE_TIME_STRING("123456");
	static_assert(cs.length() == 6);
}

TEST(StringTest, CharsAreAccessible) {
	constexpr auto cs = COMPILE_TIME_STRING("01234");
	static_assert(cs[2] == '2');
}

TEST(StringTest, CstrIsAccessible) {
	constexpr auto cs = COMPILE_TIME_STRING("01234");
	EXPECT_EQ(std::string(cs.c_str()), "01234"s);
}

TEST(StringTest, IsEqualityTestable) {
	constexpr auto cs = COMPILE_TIME_STRING("01234");
	constexpr auto same = COMPILE_TIME_STRING("01234");
	constexpr auto different = COMPILE_TIME_STRING("01_34");
	static_assert(cs == same);
	static_assert(cs != different);
}

TEST(StringTest, ShiftLeftOperatorPrintsToOstream) {
	constexpr auto cs = COMPILE_TIME_STRING("Test");
	auto oss = std::ostringstream{};
	oss << cs;
	EXPECT_EQ(oss.str(), "Test"s);
}

} // anonymous namespace
