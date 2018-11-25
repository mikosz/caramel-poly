// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/vtable.hpp"

namespace /* anonymous */ {

using namespace caramel::poly;

const auto fooName = POLY_FUNCTION_LABEL("foo");
const auto barName = POLY_FUNCTION_LABEL("bar");
const auto bazName = POLY_FUNCTION_LABEL("baz");

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
const auto caramel::poly::defaultConceptMap<Interface, T> = makeConceptMap(
	POLY_FUNCTION_LABEL("foo") = [](const S& s) { return s.i; },
	POLY_FUNCTION_LABEL("bar") = [](const S& s, int i) { return s.i * i; }
	);

template <class T>
const auto caramel::poly::conceptMap<Interface, T, std::enable_if_t<std::is_same_v<T, S>>> = makeConceptMap(
	POLY_FUNCTION_LABEL("baz") = [](S& s, double d) { s.i = static_cast<int>(d); }
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

TEST(VTableTest, OnlySelectsListedFunctions) {
	using All = detail::ConstexprList<decltype(fooName), decltype(barName), decltype(bazName)>;
	using Selector = Only<decltype(fooName), decltype(bazName)>;
	using Selected = decltype(Selector{}(All{}));

	static_assert(detail::isValidSelector<Selector>);

	static_assert(std::is_same_v<Selected::First, detail::ConstexprList<std::decay_t<decltype(barName)>>>);
	static_assert(
		std::is_same_v<
			Selected::Second,
			detail::ConstexprList<std::decay_t<decltype(fooName)>, std::decay_t<decltype(bazName)>>
			>
		);

	// Should not compile with error "Some functions specified in this selector are not part of
	// the concept to which the selector was applied"
	// using NotASubsetSelector = Only<decltype(fooName), decltype(bzzName)>;
	// using NotASubsetSelected = decltype(NotASubsetSelector{}(All{}));
}

TEST(VTableTest, ExceptSelectsListedFunctions) {
	using All = detail::ConstexprList<decltype(fooName), decltype(barName), decltype(bazName)>;
	using Selector = Except<decltype(barName)>;
	using Selected = decltype(Selector{}(All{}));

	static_assert(detail::isValidSelector<Selector>);

	static_assert(std::is_same_v<Selected::First, detail::ConstexprList<std::decay_t<decltype(barName)>>>);
	static_assert(
		std::is_same_v<
			Selected::Second,
			detail::ConstexprList<std::decay_t<decltype(bazName)>, std::decay_t<decltype(fooName)>>
			>
		);

	// Should not compile with error "Some functions specified in this selector are not part of
	// the concept to which the selector was applied"
	// using NotASubsetSelector = Except<decltype(fooName), decltype(bzzName)>;
	// using NotASubsetSelected = decltype(NotASubsetSelector{}(All{}));
}

TEST(VTableTest, EverythingSelectsAllFunctions) {
	using All = detail::ConstexprList<decltype(fooName), decltype(barName), decltype(bazName)>;
	using Selector = Everything;
	using Selected = decltype(Selector{}(All{}));

	static_assert(detail::isValidSelector<Selector>);
	static_assert(std::is_same_v<Selected::Second, All>);
}

TEST(VTableTest, LocalVTableGenerationTest) {
	using All = detail::ConstexprList<decltype(fooName), decltype(barName), decltype(bazName)>;
	using Selector = Everything;
	using Selected = decltype(Selector{}(All{}));

	static_assert(detail::isValidSelector<Selector>);
	static_assert(std::is_same_v<Selected::Second, All>);

	auto s = S{ 3 };

	const auto complete = completeConceptMap<Interface, S>(conceptMap<Interface, S>);

	using Generated = VTable<Local<Everything>>;
	const auto vtable = Generated::Type<Interface>{complete};

	static_assert(sizeof(vtable) == 3 * sizeof(void*));

	EXPECT_EQ((*vtable[fooName])(&s), 3);
	EXPECT_EQ((*vtable[barName])(&s, 2), 6);
	(*vtable[bazName])(&s, 5.5);
	EXPECT_EQ(s.i, 5);
}

TEST(VTableTest, SplitVTableGenerationTest) {
	auto s = S{ 3 };

	const auto complete = completeConceptMap<Interface, S>(conceptMap<Interface, S>);

	using Generated = VTable<Local<Only<decltype(fooName)>>, Remote<EverythingElse>>;
	const auto vtable = Generated::Type<Interface>{complete};

	static_assert(sizeof(vtable) == 2 * sizeof(void*));

	EXPECT_EQ((*vtable[fooName])(&s), 3);
	EXPECT_EQ((*vtable[barName])(&s, 2), 6);
	(*vtable[bazName])(&s, 5.5);
	EXPECT_EQ(s.i, 5);
}

} // anonymous namespace
