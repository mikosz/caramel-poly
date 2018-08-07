#include <gtest/gtest.h>

#include <utility>

#include "caramel-poly/detail/DefaultConstructibleLambda.hpp"

namespace /* anonymous */ {

using namespace caramel_poly::detail;

struct Functor {
	std::pair<int, int> operator()(int arg) const {
		return { 0, arg };
	}
};

TEST(DefaultConstructibleLambdaTest, InvokesProvidedFunctionWithArguments) {
	constexpr auto functor = DefaultConstructibleLambda<Functor, std::pair<int, int> (int)>();
	EXPECT_EQ(functor(42), std::make_pair(0, 42));

	auto lambda = [](int arg) { return std::make_pair(1, arg); };
	constexpr auto lambdaFunctor = DefaultConstructibleLambda<decltype(lambda), std::pair<int, int> (int)>();
	EXPECT_EQ(lambdaFunctor(42), std::make_pair(1, 42));
}

} // anonymous namespace
