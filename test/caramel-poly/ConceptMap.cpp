// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)

#include <gtest/gtest.h>

#include <type_traits>

#include "caramel-poly/ConceptMap.hpp"

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
	//fooName = [](const S& s) { return s.i; },
	//barName = [](const S& s, int i) { return s.i * i; }
	);

//template <>
//const auto caramel_poly::conceptMap<Interface, S> = makeConceptMap(
	//bazName = [](S& s, double d) { s.i = static_cast<int>(d); }
	//);

namespace /* anonymous */ {

TEST(ConceptMapTest, StoredFunctionsAreCallable) {
	constexpr auto complete = completeConceptMap<Interface, S>(conceptMap<Interface, S>);
}

} // anonymous namespace
