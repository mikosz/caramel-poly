// Copyright Louis Dionne 2017
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>

#include "caramel-poly/Poly.hpp"

#include <cstddef>
#include <functional>
#include <string>
#include <utility>

namespace /* anonymous */ {

constexpr auto call_LABEL = POLY_FUNCTION_LABEL("call");

//
// Example of creating a naive equivalent to `std::function` using the library.
//

template <typename Signature>
struct Callable;

template <typename R, typename ...Args>
struct Callable<R(Args...)> : decltype(caramel::poly::require(
	caramel::poly::CopyConstructible{},
	caramel::poly::MoveConstructible{},
	caramel::poly::Destructible{},
	call_LABEL = caramel::poly::function<R(caramel::poly::SelfPlaceholder const&, Args...)>
)) { };

} // anonymous namespace

template <typename R, typename ...Args, typename F>
auto const caramel::poly::defaultTraitMap<Callable<R(Args...)>, F> = caramel::poly::makeTraitMap(
	call_LABEL = [](F const& f, Args ...args) -> R {
			return f(std::forward<Args>(args)...);
		}
);

namespace /* anonymous */ {

template <typename Signature, typename StoragePolicy>
struct basic_function;

template <typename R, typename ...Args, typename StoragePolicy>
struct basic_function<R(Args...), StoragePolicy> {
	template <typename F = R(Args...)>
	basic_function(F&& f) : poly_{std::forward<F>(f)} { }

	R operator()(Args ...args) const
	{ return poly_.virtual_(call_LABEL)(poly_, std::forward<Args>(args)...); }

private:
	caramel::poly::Poly<Callable<R(Args...)>, StoragePolicy> poly_;
};

template <typename Signature>
using function = basic_function<Signature, caramel::poly::SBOStorage<16>>;

template <typename Signature>
using function_view = basic_function<Signature, caramel::poly::NonOwningStorage>;

template <typename Signature> // could also templatize the size
using inplace_function = basic_function<Signature, caramel::poly::LocalStorage<16>>;


//
// Tests
//

struct ToStringAdd {
	ToStringAdd(int num) : num_(num) { }
	std::string to_string_add(int i) const { return std::to_string(num_ + i); }
	int num_;
};

struct ToString {
	std::string operator()(int i) const { return std::to_string(i); }
};

template <template <typename> class Function>
void test() {
	// store a free function
	{
		function<std::string(int)> tostring = std::to_string;
		EXPECT_EQ(tostring(1), "1");
		EXPECT_EQ(tostring(2), "2");
		EXPECT_EQ(tostring(3), "3");
		EXPECT_EQ(tostring(-10), "-10");
	}

	// store a lambda
	{
		auto lambda = [](std::string const& s) {
			return static_cast<int>(s.size());
		};
		function<int(std::string const&)> size = lambda;

		EXPECT_EQ(size(""), 0);
		EXPECT_EQ(size("a"), 1);
		EXPECT_EQ(size("ab"), 2);
		EXPECT_EQ(size("abc"), 3);
		EXPECT_EQ(size("abcdef"), 6);
	}

	// store the result of a call to std::bind
	{
		auto bind = std::bind(static_cast<std::string(*)(int)>(std::to_string), 31337);
		function<std::string()> tostring = bind;
		EXPECT_EQ(tostring(), "31337");
	}

	// store a call to a member function and object
	{
		ToStringAdd const adder{314159};
		auto bind = std::bind(&ToStringAdd::to_string_add, adder, std::placeholders::_1);
		function<std::string(int)> f = bind;
		EXPECT_EQ(f(1), "314160");
		EXPECT_EQ(f(2), "314161");
		EXPECT_EQ(f(3), "314162");
		EXPECT_EQ(f(-10), "314149");
	}

	// store a call to a member function and object ptr
	{
		ToStringAdd const adder{314159};
		auto bind = std::bind(&ToStringAdd::to_string_add, &adder, std::placeholders::_1);
		function<std::string(int)> f = bind;
		EXPECT_EQ(f(1), "314160");
		EXPECT_EQ(f(2), "314161");
		EXPECT_EQ(f(3), "314162");
		EXPECT_EQ(f(-10), "314149");
	}

	// store a call to a function object
	{
		ToString f{};
		function<std::string(int)> tostring = f;
		EXPECT_EQ(tostring(1), "1");
		EXPECT_EQ(tostring(2), "2");
		EXPECT_EQ(tostring(3), "3");
		EXPECT_EQ(tostring(18), "18");
	}
}

TEST(DynoTest, FunctionExample) {
	test<function>();
	test<function_view>();
	test<inplace_function>();
}

} // anonymous namespace
