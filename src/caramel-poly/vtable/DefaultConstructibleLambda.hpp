#ifndef CARAMELPOLY_VTABLE_DEFAULTCONSTRUCTIBLELAMBDA_HPP__
#define CARAMELPOLY_VTABLE_DEFAULTCONSTRUCTIBLELAMBDA_HPP__

#include "caramel-poly/detail/EmptyObject.hpp"

namespace caramel_poly::vtable {

template <class LambdaType, class Signature>
class DefaultConstructibleLambda;

template <class LambdaType, class ReturnType, class... Args>
class DefaultConstructibleLambda<LambdaType, ReturnType (Args...)> {
public:

	using Signature = ReturnType (Args...);

	ReturnType operator()(Args... args) {
		const auto lambda = detail::EmptyObject<LambdaType>{}.get();
		return lambda(std::forward<Args>(args)...);
	}

};

template <class LambdaType, class... Args>
class DefaultConstructibleLambda<LambdaType, void (Args...)> {
public:

	using Signature = void (Args...);

	ReturnType operator()(Args... args) {
		const auto lambda = detail::EmptyObject<LambdaType>{}.get();
		lambda(std::forward<Args>(args)...);
	}

};

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_DEFAULTCONSTRUCTIBLELAMBDA_HPP__ */
