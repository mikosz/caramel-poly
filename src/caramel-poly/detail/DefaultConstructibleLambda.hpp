// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_DETAIL_DEFAULTCONSTRUCTIBLELAMBDA_HPP__
#define CARAMELPOLY_DETAIL_DEFAULTCONSTRUCTIBLELAMBDA_HPP__

#include "caramel-poly/detail/EmptyObject.hpp"

namespace caramel::poly::detail {

template <class LambdaType, class Signature>
struct DefaultConstructibleLambda;

template <class LambdaType, class ReturnType, class... Args>
struct DefaultConstructibleLambda<LambdaType, ReturnType (Args...)> {

	using Signature = ReturnType (Args...);

	ReturnType operator()(Args... args) const {
		const auto lambda = caramel::poly::detail::EmptyObject<LambdaType>{}.get();
		return lambda(std::forward<Args>(args)...);
	}

};

template <class LambdaType, class... Args>
struct DefaultConstructibleLambda<LambdaType, void (Args...)> {

	using Signature = void (Args...);

	void operator()(Args... args) const {
		const auto lambda = caramel::poly::detail::EmptyObject<LambdaType>{}.get();
		lambda(std::forward<Args>(args)...);
	}

};

} // namespace caramel::poly::detail

#endif /* CARAMELPOLY_DETAIL_DEFAULTCONSTRUCTIBLELAMBDA_HPP__ */
