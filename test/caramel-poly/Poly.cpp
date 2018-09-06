// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <string>

#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace std::string_literals;

constexpr auto CONST_PRINT_NAME = METHOD_NAME("cprint");
constexpr auto NONCONST_PRINT_NAME = METHOD_NAME("ncprint");
constexpr auto FREE_PRINT_NAME = METHOD_NAME("print");

struct Printable : decltype(requires(
	CONST_PRINT_NAME = method<std::string () const>,
	NONCONST_PRINT_NAME = method<std::string ()>,
	FREE_PRINT_NAME = function<std::string (const caramel_poly::SelfPlaceholder&)>
	))
{
};

struct S {
	int i;

	std::string print() const {
		return "cprint:S:"s + std::to_string(i);
	}

	std::string print() {
		return "ncprint:S:"s + std::to_string(i);
	}

};

std::string print(const S& s) {
	return "fprint:S:" + std::to_string(s.i);
}

struct T {
	float f;

	std::string print() const {
		return "cprint:T:"s + std::to_string(f);
	}

	std::string print() {
		return "ncprint:T:"s + std::to_string(f);
	}

};

std::string print(const T& t) {
	return "fprint:T:" + std::to_string(t.f);
}

} // anonymous namespace

template <class T>
constexpr auto caramel_poly::defaultConceptMap<Printable, T> = makeConceptMap(
	CONST_PRINT_NAME = [](const auto& o) { return o.print(); },
	NONCONST_PRINT_NAME = [](auto& o) { return o.print(); },
	FREE_PRINT_NAME = [](const auto& o) { return print(o); }
	);

namespace /* anonymous */ {

TEST(PolyTest, InvokesBasicPolymorphicCalls) {
	auto sp = Poly<Printable>(S{ 42 });
	EXPECT_EQ(sp.virtual_(CONST_PRINT_NAME)(), "cprint:S:42"s);
	EXPECT_EQ(sp.virtual_(NONCONST_PRINT_NAME)(), "ncprint:S:42"s);
	EXPECT_EQ(sp.virtual_(FREE_PRINT_NAME)(sp), "fprint:S:42"s);

	sp = Poly<Printable>(T{ 3.14f });
	EXPECT_EQ(sp.virtual_(CONST_PRINT_NAME)(), "cprint:T:"s + std::to_string(3.14f));
	EXPECT_EQ(sp.virtual_(NONCONST_PRINT_NAME)(), "ncprint:T:"s + std::to_string(3.14f));
	EXPECT_EQ(sp.virtual_(FREE_PRINT_NAME)(sp), "fprint:T:"s + std::to_string(3.14f));
}

} // anonymous namespace
