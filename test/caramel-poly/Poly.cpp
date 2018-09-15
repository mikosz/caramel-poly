// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <string>

#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;
using namespace std::string_literals;

constexpr auto CONST_PRINT_NAME = POLY_FUNCTION_LABEL("cprint");
constexpr auto NONCONST_PRINT_NAME = POLY_FUNCTION_LABEL("ncprint");
constexpr auto FREE_PRINT_NAME = POLY_FUNCTION_LABEL("print");

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

template <class T>
constexpr auto caramel_poly::conceptMap<Printable, T, std::enable_if_t<std::is_same_v<int, T>>> = makeConceptMap(
	CONST_PRINT_NAME = [](const auto& o) { return "cprint:int:"s + std::to_string(o); },
	NONCONST_PRINT_NAME = [](auto& o) { return "ncprint:int:"s + std::to_string(o); },
	FREE_PRINT_NAME = [](const auto& o) { return "fprint:int:"s + std::to_string(o); }
	);

namespace /* anonymous */ {

TEST(PolyTest, InvokesBasicPolymorphicCalls) {
	auto sp = Poly<Printable>(S{ 42 });
	EXPECT_EQ(sp.virtual_(CONST_PRINT_NAME)(), "cprint:S:42"s);
	EXPECT_EQ(sp.virtual_(NONCONST_PRINT_NAME)(), "ncprint:S:42"s);
	EXPECT_EQ(sp.virtual_(FREE_PRINT_NAME)(sp), "fprint:S:42"s);
	EXPECT_EQ(sp.invoke(CONST_PRINT_NAME), "cprint:S:42"s);
	EXPECT_EQ(sp.invoke(NONCONST_PRINT_NAME), "ncprint:S:42"s);
	EXPECT_EQ(sp.invoke(FREE_PRINT_NAME, sp), "fprint:S:42"s);

	sp = Poly<Printable>(T{ 3.14f });
	EXPECT_EQ(sp.virtual_(CONST_PRINT_NAME)(), "cprint:T:"s + std::to_string(3.14f));
	EXPECT_EQ(sp.virtual_(NONCONST_PRINT_NAME)(), "ncprint:T:"s + std::to_string(3.14f));
	EXPECT_EQ(sp.virtual_(FREE_PRINT_NAME)(sp), "fprint:T:"s + std::to_string(3.14f));
	EXPECT_EQ(sp.invoke(CONST_PRINT_NAME), "cprint:T:"s + std::to_string(3.14f));
	EXPECT_EQ(sp.invoke(NONCONST_PRINT_NAME), "ncprint:T:"s + std::to_string(3.14f));
	EXPECT_EQ(sp.invoke(FREE_PRINT_NAME, sp), "fprint:T:"s + std::to_string(3.14f));

	sp = Poly<Printable>(12);
	EXPECT_EQ(sp.virtual_(CONST_PRINT_NAME)(), "cprint:int:12"s);
	EXPECT_EQ(sp.virtual_(NONCONST_PRINT_NAME)(), "ncprint:int:12"s);
	EXPECT_EQ(sp.virtual_(FREE_PRINT_NAME)(sp), "fprint:int:12"s);
	EXPECT_EQ(sp.invoke(CONST_PRINT_NAME), "cprint:int:12"s);
	EXPECT_EQ(sp.invoke(NONCONST_PRINT_NAME), "ncprint:int:12"s);
	EXPECT_EQ(sp.invoke(FREE_PRINT_NAME, sp), "fprint:int:12"s);
}

TEST(PolyTest, StorableAndDestructibleByDefault) {
	auto sp = Poly<Printable>(S{ 42 });

	static_assert(models<Storable, S>);
	static_assert(models<Destructible, S>);

	EXPECT_EQ(sp.virtual_(STORAGE_INFO_LABEL)().size, sizeof(S));
	EXPECT_EQ(sp.virtual_(STORAGE_INFO_LABEL)().alignment, alignof(S));
	sp.virtual_(DESTRUCT_LABEL);
}

} // anonymous namespace
