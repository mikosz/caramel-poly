// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <string>

#include "caramel-poly/Poly.hpp"

namespace /* anonymous */ {

using namespace caramel::poly;
using namespace std::string_literals;

constexpr auto CONST_PRINT_NAME = POLY_FUNCTION_LABEL("cprint");
constexpr auto NONCONST_PRINT_NAME = POLY_FUNCTION_LABEL("ncprint");
constexpr auto FREE_PRINT_NAME = POLY_FUNCTION_LABEL("print");

struct Printable : decltype(require(
	CONST_PRINT_NAME = method<std::string () const>,
	NONCONST_PRINT_NAME = method<std::string ()>,
	FREE_PRINT_NAME = function<std::string (const caramel::poly::SelfPlaceholder&)>
	))
{
};

// TODO: Previously this struct was called S and WithFloat was called T. This worked in
// most situations, but for some reason on clang in debug builds this was breaking
// storage info tests, where the size and alignment of this struct would be returned
// when reading it for builtin.cpp typeid test. Since these classes are in anonymous
// namespaces there is no reason for this to happen and yet it does. This is probably
// a configuration issue, but worth investigating.
struct WithInt {
	int i;

	std::string print() const {
		return "cprint:S:"s + std::to_string(i);
	}

	std::string print() {
		return "ncprint:S:"s + std::to_string(i);
	}

};

std::string print(const WithInt& s) {
	return "fprint:S:" + std::to_string(s.i);
}

struct WithFloat {
	float f;

	std::string print() const {
		return "cprint:T:"s + std::to_string(f);
	}

	std::string print() {
		return "ncprint:T:"s + std::to_string(f);
	}

};

std::string print(const WithFloat& t) {
	return "fprint:T:" + std::to_string(t.f);
}

} // anonymous namespace

template <class T>
constexpr auto caramel::poly::defaultTraitMap<Printable, T> = makeTraitMap(
	CONST_PRINT_NAME = [](const auto& o) { return o.print(); },
	NONCONST_PRINT_NAME = [](auto& o) { return o.print(); },
	FREE_PRINT_NAME = [](const auto& o) { return print(o); }
	);

template <class T>
constexpr auto caramel::poly::conceptMap<Printable, T, std::enable_if_t<std::is_same_v<int, T>>> = makeTraitMap(
	CONST_PRINT_NAME = [](const auto& o) { return "cprint:int:"s + std::to_string(o); },
	NONCONST_PRINT_NAME = [](auto& o) { return "ncprint:int:"s + std::to_string(o); },
	FREE_PRINT_NAME = [](const auto& o) { return "fprint:int:"s + std::to_string(o); }
	);

namespace /* anonymous */ {

TEST(PolyTest, InvokesBasicPolymorphicCalls) {
	auto sp = Poly<Printable>(WithInt{ 42 });
	EXPECT_EQ(sp.virtual_(CONST_PRINT_NAME)(), "cprint:S:42"s);
	EXPECT_EQ(sp.virtual_(NONCONST_PRINT_NAME)(), "ncprint:S:42"s);
	EXPECT_EQ(sp.virtual_(FREE_PRINT_NAME)(sp), "fprint:S:42"s);
	EXPECT_EQ(sp.invoke(CONST_PRINT_NAME), "cprint:S:42"s);
	EXPECT_EQ(sp.invoke(NONCONST_PRINT_NAME), "ncprint:S:42"s);
	EXPECT_EQ(sp.invoke(FREE_PRINT_NAME, sp), "fprint:S:42"s);

	sp = Poly<Printable>(WithFloat{ 3.14f });
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
	auto sp = Poly<Printable>(WithInt{ 42 });

	static_assert(models<Storable, WithInt>);
	static_assert(models<Destructible, WithInt>);

	EXPECT_EQ(sp.virtual_(STORAGE_INFO_LABEL)().size, sizeof(WithInt));
	EXPECT_EQ(sp.virtual_(STORAGE_INFO_LABEL)().alignment, alignof(WithInt));
	sp.virtual_(DESTRUCT_LABEL);
}

} // anonymous namespace
