// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "caramel-poly/detail/ConstexprString.hpp"

namespace /* anonymous */ {

using namespace std::string_literals;
using namespace caramel_poly;
using namespace caramel_poly::detail;

TEST(StringTest, HasLength) {
	constexpr auto cs = CONSTEXPR_STRING("123456");
	static_assert(cs.length() == 6);
}

TEST(StringTest, CharsAreAccessible) {
	constexpr auto cs = CONSTEXPR_STRING("01234");
	static_assert(cs[2] == '2');
}

TEST(StringTest, CstrIsAccessible) {
	constexpr auto cs = CONSTEXPR_STRING("01234");
	EXPECT_EQ(std::string(cs.c_str()), "01234"s);
}

TEST(StringTest, IsEqualityTestable) {
	constexpr auto cs = CONSTEXPR_STRING("01234");
	constexpr auto same = CONSTEXPR_STRING("01234");
	constexpr auto different = CONSTEXPR_STRING("01_34");
	static_assert(cs == same);
	static_assert(cs != different);
}

TEST(StringTest, ShiftLeftOperatorPrintsToOstream) {
	constexpr auto cs = CONSTEXPR_STRING("Test");
	auto oss = std::ostringstream{};
	oss << cs;
	EXPECT_EQ(oss.str(), "Test"s);
}

} // anonymous namespace
