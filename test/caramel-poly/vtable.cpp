// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#if 0

#include <type_traits>

#include "caramel-poly/vtable.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

const auto fooName = METHOD_NAME("foo");
const auto barName = METHOD_NAME("bar");
const auto bazName = METHOD_NAME("baz");

struct Parent : decltype(requires(
	fooName = method<int () const>
	))
{
};

struct Interface : decltype(requires(
	Parent{},
	barName = method<int (int) const>,
	bazName = method<void (double)>
	))
{
};

struct S {
	int i;
};

} // anonymous namespace

template <class T>
const auto caramel_poly::defaultConceptMap<Interface, T> = makeConceptMap(
	METHOD_NAME("foo") = [](const S& s) { return s.i; },
	METHOD_NAME("bar") = [](const S& s, int i) { return s.i * i; }
	);

template <class T>
const auto caramel_poly::conceptMap<Interface, T, std::enable_if_t<std::is_same_v<T, S>>> = makeConceptMap(
	METHOD_NAME("baz") = [](S& s, double d) { s.i = static_cast<int>(d); }
	);

namespace /* anonymous */ {

TEST(VTableTest, StoredFunctionsAreAccessible) {
	auto s = S{ 3 };

	const auto complete = completeConceptMap<Interface, S>(conceptMap<Interface, S>);
	const auto vtable = LocalVTable<
		detail::ConstexprPair<std::decay_t<decltype(fooName)>, decltype(Interface{}.getSignature(fooName)) > ,
		detail::ConstexprPair<std::decay_t<decltype(barName)>, decltype(Interface{}.getSignature(barName)) > ,
		detail::ConstexprPair<std::decay_t<decltype(bazName)>, decltype(Interface{}.getSignature(bazName))>
		>(complete);

	EXPECT_TRUE(vtable.contains(fooName));
	auto* fooPtr = vtable[fooName];
	(*fooPtr)(&s);

	EXPECT_EQ(sizeof(vtable), 3 * sizeof(void*));
}

} // anonymous namespace
#endif