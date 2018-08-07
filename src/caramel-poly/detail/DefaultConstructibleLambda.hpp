#ifndef CARAMELPOLY_DETAIL_DEFAULTCONSTRUCTIBLELAMBDA_HPP__
#define CARAMELPOLY_DETAIL_DEFAULTCONSTRUCTIBLELAMBDA_HPP__

#include "caramel-poly/detail/EmptyObject.hpp"

namespace caramel_poly::detail {

template <class LambdaType, class Signature>
struct DefaultConstructibleLambda;

template <class LambdaType, class ReturnType, class... Args>
struct DefaultConstructibleLambda<LambdaType, ReturnType (Args...)> {

	using Signature = ReturnType (Args...);

	ReturnType operator()(Args... args) const {
		const auto lambda = caramel_poly::detail::EmptyObject<LambdaType>{}.get();
		return lambda(std::forward<Args>(args)...);
	}

};

template <class LambdaType, class... Args>
struct DefaultConstructibleLambda<LambdaType, void (Args...)> {

	using Signature = void (Args...);

	void operator()(Args... args) const {
		const auto lambda = caramel_poly::detail::EmptyObject<LambdaType>{}.get();
		lambda(std::forward<Args>(args)...);
	}

};

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_DEFAULTCONSTRUCTIBLELAMBDA_HPP__ */
