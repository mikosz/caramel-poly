// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include "any_iterator.hpp"

#include "caramel-poly/Poly.hpp"

#include <algorithm>
#include <iterator>
#include <list>
#include <utility>
#include <vector>

//
// Example of creating an erased range type for holding anything that has
// begin() and end() functions.
//

namespace /* anonymous */ {

constexpr auto begin_LABEL = POLY_FUNCTION_LABEL("begin");
constexpr auto end_LABEL = POLY_FUNCTION_LABEL("end");
constexpr auto cbegin_LABEL = POLY_FUNCTION_LABEL("cbegin");
constexpr auto cend_LABEL = POLY_FUNCTION_LABEL("cend");

template <typename Value, typename Category>
struct Range : decltype(caramel_poly::requires(
	begin_LABEL = caramel_poly::function<any_iterator<Value, Category>(caramel_poly::SelfPlaceholder&)>,
	end_LABEL = caramel_poly::function<any_iterator<Value, Category>(caramel_poly::SelfPlaceholder&)>,
	cbegin_LABEL = caramel_poly::function<any_iterator<Value const, Category>(caramel_poly::SelfPlaceholder const&)>,
	cend_LABEL = caramel_poly::function<any_iterator<Value const, Category>(caramel_poly::SelfPlaceholder const&)>
)) { };

} // anonymous namespace

template <typename Value, typename Category, typename R>
auto const caramel_poly::defaultConceptMap<Range<Value, Category>, R> = caramel_poly::makeConceptMap(
		begin_LABEL = [](R& range) -> any_iterator<Value, Category> {
			return any_iterator<Value, Category>{range.begin()};
		},
	end_LABEL = [](R& range) -> any_iterator<Value, Category> {
			return any_iterator<Value, Category>{range.end()};
		},
	cbegin_LABEL = [](R const& range) -> any_iterator<Value const, Category> {
			return any_iterator<Value const, Category>{range.cbegin()};
		},
	cend_LABEL = [](R const& range) -> any_iterator<Value const, Category> {
			return any_iterator<Value const, Category>{range.cend()};
		}
	);

namespace /* anonymous */ {

template <typename Value, typename Category>
struct any_range {
	template <typename Range>
	any_range(Range&& r) : poly_{std::forward<Range>(r)} { }

	auto begin()        { return poly_.virtual_(begin_LABEL)(poly_); }
	auto end()          { return poly_.virtual_(end_LABEL)(poly_); }
	auto begin() const  { return cbegin(); }
	auto end() const    { return cend(); }

	auto cbegin() const { return poly_.virtual_(cbegin_LABEL)(poly_); }
	auto cend() const   { return poly_.virtual_(cend_LABEL)(poly_); }

private:
	caramel_poly::Poly<Range<Value, Category>> poly_;
};

//
// Tests
//

TEST(DynoTest, AnyRangeExample) {
	using Range = any_range<int, std::forward_iterator_tag>;
	{
		Range vector = std::vector<int>{1, 2, 3, 4, 5};
		Range list = std::list<int>{1, 2, 3, 4, 5};
		EXPECT_TRUE(std::equal(vector.begin(), vector.end(), list.begin(), list.end()));
	}
	{
		Range const cvector = std::vector<int>{1, 2, 3, 4, 5};
		Range const clist = std::list<int>{1, 2, 3, 4, 5};
		EXPECT_TRUE(std::equal(cvector.begin(), cvector.end(), clist.begin(), clist.end()));
	}
	{
		Range vector = std::vector<int>{1, 2, 3, 4, 5};
		Range list = std::list<int>{1, 2, 3, 4, 5};
		EXPECT_TRUE(std::equal(vector.cbegin(), vector.cend(), list.cbegin(), list.cend()));
	}
}

} // anonymous namespace
