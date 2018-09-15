// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>


//////////////////////////////////////////////////////////////////////////////
// Important: Keep this file in sync with the 'Erasing non-member functions'
//            section of the README
//////////////////////////////////////////////////////////////////////////////
#include "caramel-poly/Poly.hpp"
#include <iostream>
#include <sstream>

namespace /* anonymous */ {

constexpr auto draw_LABEL = POLY_FUNCTION_LABEL("draw");

// Define the interface of something that can be drawn
struct Drawable : decltype(caramel_poly::requires(
	draw_LABEL = caramel_poly::function<void(std::ostream&, caramel_poly::SelfPlaceholder const&)>
)) { };

} // anonymous namespace

// Define how concrete types can fulfill that interface
template <typename T>
auto const caramel_poly::defaultConceptMap<Drawable, T> = caramel_poly::makeConceptMap(
	draw_LABEL = [](std::ostream& out, T const& self) { self.draw(out); }
	//            ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ matches the concept definition
	);

namespace /* anonymous */ {

// Define an object that can hold anything that can be drawn.
struct drawable {
	template <typename T>
	drawable(T x) : poly_{ x } { }

	void draw(std::ostream& out) const
	{
		poly_.virtual_(draw_LABEL)(out, poly_);
	}
	//                              ^^^^^ passing the poly explicitly

private:
	caramel_poly::Poly<Drawable> poly_;
};

struct Square {
	void draw(std::ostream& out) const { out << "Square\n"; }
};

struct Circle {
	void draw(std::ostream& out) const { out << "Circle\n"; }
};

void f(drawable const& d, std::ostream& os) {
	d.draw(os);
}

TEST(DynoTest, ReadmeNonmemberExample) {
	auto oss = std::ostringstream{};
	f(Square{}, oss); // prints Square
	f(Circle{}, oss); // prints Circle
	EXPECT_EQ(oss.str(),
R"(Square
Circle
)"
		);
}
//////////////////////////////////////////////////////////////////////////////

} // anonymous namespace
