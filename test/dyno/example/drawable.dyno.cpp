// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Poly.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <sstream>

namespace /* anonymous */ {

constexpr auto draw_LABEL = POLY_FUNCTION_LABEL("draw");

template <typename Object>
using document_t = std::vector<Object>;

template <typename Document>
using history_t = std::vector<Document>;

template <typename T>
void draw(T const& t, std::ostream& out)
{
	out << t << std::endl;
}

template <typename Document>
void commit(history_t<Document>& x) {
	assert(!x.empty());
	x.push_back(x.back());
}

template <typename Document>
void undo(history_t<Document>& x) {
	assert(!x.empty());
	x.pop_back();
}

template <typename Document>
Document& current(history_t<Document>& x) {
	assert(!x.empty());
	return x.back();
}

struct my_class_t { };
void draw(my_class_t /*self*/, std::ostream& out)
{
	out << "my_class_t" << std::endl;
}

template <typename Object>
void draw(document_t<Object> const& self, std::ostream& out) {
	out << "<document>" << std::endl;
	for (auto const& x : self)
		draw(x, out);
	out << "</document>" << std::endl;
}



struct Drawable : decltype(caramel_poly::requires(
	draw_LABEL = caramel_poly::function<void(caramel_poly::SelfPlaceholder const&, std::ostream&)>
)) { };

class object_t {
public:
	template <typename T>
	object_t(T x)
		: poly_{ std::move(x), caramel_poly::makeConceptMap(
			draw_LABEL = [](T const& self, std::ostream& out) { draw(self, out); }
		) }
	{ }

	friend void draw(object_t const& x, std::ostream& out) {
		x.poly_.virtual_(draw_LABEL)(x.poly_, out);
	}

private:
	using Storage = caramel_poly::SharedRemoteStorage;
	using VTable = caramel_poly::VTable<caramel_poly::Remote<caramel_poly::Everything>>;
	caramel_poly::Poly<Drawable, Storage, VTable> poly_;
};

TEST(DynoTest, DrawableDynoExample) {
	history_t<document_t<object_t>> h{1};
	current(h).emplace_back(0);
	current(h).emplace_back(std::string{"Hello!"});

	auto oss = std::ostringstream();

	draw(current(h), oss);
	oss << "-------------------" << std::endl;

	commit(h);

	current(h).emplace_back(current(h));
	current(h).emplace_back(my_class_t{});
	current(h)[1] = std::string{"World"};

	draw(current(h), oss);
	oss << "-------------------" << std::endl;

	undo(h);

	draw(current(h), oss);

	EXPECT_EQ(
		oss.str(),
		R"(<document>
0
Hello!
</document>
-------------------
<document>
0
World
<document>
0
Hello!
</document>
my_class_t
</document>
-------------------
<document>
0
Hello!
</document>
)"
		);
}

} // anonymous namespace
