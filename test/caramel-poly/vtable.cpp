// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/vtable.hpp"

namespace /* anonymous */ {

using namespace caramel_poly;

const auto fooName = METHOD_NAME("foo");
const auto barName = METHOD_NAME("bar");
const auto bazName = METHOD_NAME("baz");
const auto bzzName = METHOD_NAME("bzz");

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

TEST(VTableTest, LocalVTableStoredFunctionsAreAccessible) {
	auto s = S{ 3 };

	const auto complete = completeConceptMap<Interface, S>(conceptMap<Interface, S>);
	const auto vtable = LocalVTable<
		detail::ConstexprPair<std::decay_t<decltype(fooName)>, decltype(Interface{}.getSignature(fooName))>,
		detail::ConstexprPair<std::decay_t<decltype(barName)>, decltype(Interface{}.getSignature(barName))>,
		detail::ConstexprPair<std::decay_t<decltype(bazName)>, decltype(Interface{}.getSignature(bazName))>
		>{ complete };

	static_assert(sizeof(vtable) == 3 * sizeof(void*));

	EXPECT_TRUE(vtable.contains(fooName));
	EXPECT_EQ((*vtable[fooName])(&s), 3);
	EXPECT_EQ((*vtable[barName])(&s, 2), 6);
	(*vtable[bazName])(&s, 42.12);
	EXPECT_EQ(s.i, 42);
	// Should not compile with error "Request for a virtual function that is not in the vtable"
	// vtable[bzzName];
}

TEST(VTableTest, RemoteVTableStoredFunctionsAreAccessible) {
	auto s = S{ 3 };

	const auto complete = completeConceptMap<Interface, S>(conceptMap<Interface, S>);
	using Local = LocalVTable<
		detail::ConstexprPair<std::decay_t<decltype(fooName)>, decltype(Interface{}.getSignature(fooName))>,
		detail::ConstexprPair<std::decay_t<decltype(barName)>, decltype(Interface{}.getSignature(barName))>,
		detail::ConstexprPair<std::decay_t<decltype(bazName)>, decltype(Interface{}.getSignature(bazName))>
		>;
	const auto vtable = RemoteVTable<Local>{ complete };

	static_assert(sizeof(vtable) == sizeof(void*));

	EXPECT_TRUE(vtable.contains(fooName));
	EXPECT_EQ((*vtable[fooName])(&s), 3);
	EXPECT_EQ((*vtable[barName])(&s, 2), 6);
	(*vtable[bazName])(&s, 42.12);
	EXPECT_EQ(s.i, 42);
	// Should not compile with error "Request for a virtual function that is not in the vtable"
	// vtable[bzzName];
}

TEST(VTableTest, JoinedVTableStoredFunctionsAreAccessible) {
	auto s = S{ 3 };

	const auto complete = completeConceptMap<Interface, S>(conceptMap<Interface, S>);
	using FooBar = LocalVTable<
		detail::ConstexprPair<std::decay_t<decltype(fooName)>, decltype(Interface{}.getSignature(fooName))>,
		detail::ConstexprPair<std::decay_t<decltype(barName)>, decltype(Interface{}.getSignature(barName))>
		>;
	using BarBaz = LocalVTable<
		detail::ConstexprPair<std::decay_t<decltype(barName)>, decltype(Interface{}.getSignature(barName))>,
		detail::ConstexprPair<std::decay_t<decltype(bazName)>, decltype(Interface{}.getSignature(bazName))>
		>;
	using RemoteBarBaz = RemoteVTable<BarBaz>;

	const auto vtable = JoinedVTable<FooBar, RemoteBarBaz>{complete};

	static_assert(sizeof(vtable) == 2 * sizeof(void*) + sizeof(void*));

	EXPECT_TRUE(vtable.contains(fooName));
	EXPECT_EQ((*vtable[fooName])(&s), 3);
	// Should not compile with error "Request for a virtual function that is contained
	// in both vtables of a joined vtable"
	// EXPECT_EQ((*vtable[barName])(&s, 2), 6);
	(*vtable[bazName])(&s, 42.12);
	EXPECT_EQ(s.i, 42);
	// Should not compile with error "Request for a virtual function that is not present in any
	// of the joined tables"
	// vtable[bzzName];
}

} // anonymous namespace
