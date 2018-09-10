// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "any_iterator.hpp"

#include <array>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

namespace /* anonymous */ {

TEST(DynoTest, AnyIteratorExample) {
	////////////////////////////////////////////////////////////////////////////
	// Iteration
	////////////////////////////////////////////////////////////////////////////
	{
		using Iterator = any_iterator<int, std::forward_iterator_tag>;
		std::vector<int> input{ 1, 2, 3, 4 };
		std::vector<int> result;

		Iterator first{ input.begin() }, last{ input.end() };
		for (; first != last; ++first) {
			result.push_back(*first);
		}
		EXPECT_EQ(result, input);
	}

	{
		using Iterator = any_iterator<int, std::bidirectional_iterator_tag>;
		std::array<int, 4> input{ {1, 2, 3, 4} };
		std::array<int, 4> result;
		std::reverse_iterator<Iterator> first{ Iterator{input.end()} },
			last{ Iterator{input.begin()} };
		Iterator out{ result.begin() };

		for (; first != last; ++first, ++out) {
			*out = *first;
		}
		std::array<int, 4> expected{ {4, 3, 2, 1} };
		EXPECT_EQ(result, expected);
	}

	{
		using Iterator = any_iterator<int, std::random_access_iterator_tag>;
		std::array<int, 4> input{ {1, 2, 3, 4} };
		std::array<int, 4> result;
		Iterator first{ input.begin() }, last{ input.end() }, out{ result.begin() };

		for (; first != last; ++first, ++out) {
			*out = *first;
		}
		EXPECT_EQ(result, input);
	}

	////////////////////////////////////////////////////////////////////////////
	// Copy-construction
	////////////////////////////////////////////////////////////////////////////
	{
		using Iterator = any_iterator<int, std::random_access_iterator_tag>;
		std::vector<int> input{ 1, 2, 3, 4 };
		Iterator first{ input.begin() }, last{ input.end() };
		Iterator cfirst(first), clast(last);
		EXPECT_EQ(first, cfirst);
		EXPECT_EQ(last, clast);
	}

	////////////////////////////////////////////////////////////////////////////
	// Move-construction
	////////////////////////////////////////////////////////////////////////////
	{
		using Iterator = any_iterator<int, std::random_access_iterator_tag>;
		std::vector<int> input{ 1, 2, 3, 4 };
		std::vector<int> result;

		Iterator first{ input.begin() }, last{ input.end() };
		Iterator cfirst(std::move(first)), clast(std::move(last));
		for (; cfirst != clast; ++cfirst) {
			result.push_back(*cfirst);
		}
		EXPECT_EQ(result, input);
	}

	////////////////////////////////////////////////////////////////////////////
	// Copy-assignment
	////////////////////////////////////////////////////////////////////////////
	{
		using Iterator = any_iterator<int, std::random_access_iterator_tag>;
		std::vector<int> input{ 1, 2, 3, 4 };
		Iterator first{ input.begin() }, mid{ input.begin() + 2 };
		EXPECT_EQ(*first, 1);
		first = mid;
		EXPECT_EQ(*first, 3);
	}

	////////////////////////////////////////////////////////////////////////////
	// Move-assignment
	////////////////////////////////////////////////////////////////////////////
	{
		using Iterator = any_iterator<int, std::random_access_iterator_tag>;
		std::vector<int> input{ 1, 2, 3, 4 };
		Iterator first{ input.begin() }, mid{ input.begin() + 2 };
		EXPECT_EQ(*first, 1);
		first = std::move(mid);
		EXPECT_EQ(*first, 3);
	}

	////////////////////////////////////////////////////////////////////////////
	// Swap
	////////////////////////////////////////////////////////////////////////////
	{
		using Iterator = any_iterator<int, std::random_access_iterator_tag>;
		std::vector<int> input{ 1, 2, 3, 4 };
		Iterator first{ input.begin() }, last{ input.end() };
		using std::swap;
		swap(first, last);
		EXPECT_EQ(*last, 1);
		++last;
		EXPECT_EQ(*last, 2);
		EXPECT_EQ(first, Iterator{ input.end() });
	}
}

} // anonymous namespace
