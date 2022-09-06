// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Poly.hpp"

#include <iostream>
#include <type_traits>
#include <vector>

namespace /* anonymous */ {

constexpr auto draw_LABEL = POLY_FUNCTION_LABEL("draw");

struct Drawable : decltype(caramel::poly::require(
	draw_LABEL = caramel::poly::method<void(std::ostream&) const>
)) { };

} // anonymous namespace

template <typename T>
auto const caramel::poly::defaultTraitMap<Drawable, T> = caramel::poly::makeTraitMap(
	draw_LABEL = [](auto const& self, std::ostream& out) { self.draw(out); }
);

namespace /* anonymous */ {

struct drawable {
	template <typename T>
	drawable(T x) : poly_{x} { }

	void draw(std::ostream& out) const
	{ poly_.virtual_(draw_LABEL)(out); }

private:
	caramel::poly::Poly<Drawable> poly_;
};


struct Square { /* ... */ };

} // anonymous namespace

template <class T>
auto const caramel::poly::conceptMap<Drawable, T, std::enable_if_t<std::is_same_v<T, Square>>> = caramel::poly::makeTraitMap(
	draw_LABEL = [](Square const& /*square*/, std::ostream& out) {
	out << "square" << std::endl;
}
);

namespace /* anonymous */ {

struct Circle {
	void draw(std::ostream& out) const {
		out << "circle" << std::endl;
	}
};

} // anonymous namespace

template <typename T>
auto const caramel::poly::conceptMap<Drawable, std::vector<T>, std::void_t<decltype(
	std::cout << std::declval<T>()
	)>> = caramel::poly::makeTraitMap(
		draw_LABEL = [](std::vector<T> const& v, std::ostream& out) {
	for (auto const& x : v)
		out << x << ' ';
	out << std::endl;
}
);

namespace /* anonymous */ {

void f(drawable d, std::ostringstream& os) {
	d.draw(os);
}

TEST(DynoTest, ReadmeExample) {
	auto oss = std::ostringstream{};
	f(Square{}, oss);                  // prints "square"
	f(Circle{}, oss);                  // prints "circle"
	f(std::vector<int>{1, 2, 3}, oss); // prints "1 2 3 "
	EXPECT_EQ(oss.str(),
R"(square
circle
1 2 3 
)"
		);
}

} // anonymous namespace
