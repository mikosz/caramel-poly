#ifndef CARAMELPOLY_DETAIL_METHOD_HPP__
#define CARAMELPOLY_DETAIL_METHOD_HPP__

#include <type_traits>

#include "caramel-poly/vtable/Object.hpp"
#include "caramel-poly/vtable/MethodSignature.hpp"

namespace caramel_poly::detail {

template <class MappingSignature>
class Method;

template <class MappingReturnType, class ObjectArg, class... MappingArgs>
class Method<MappingReturnType (ObjectArg, MappingArgs...)> {
public:

	using FunctionPtr = MappingReturnType (*)(ObjectArg, MappingArgs...);

	template <class DefaultConstructibleLambdaType>
	constexpr Method([[maybe_unused]] DefaultConstructibleLambdaType default_constructible_lambda) :
		function_(
			&Uneraser<
				DefaultConstructibleLambdaType,
				typename DefaultConstructibleLambdaType::Signature,
				MappingArgs...
				>::invoke
			)
	{
	}

	template <class SelfType, class... Args>
	decltype(auto) invoke(SelfType&& self, Args... args) const {
		return (*function_)(reinterpret_cast<ObjectArg>(self), std::forward<Args>(args)...);
	}

private:

	template <class SelfType, class ObjectArg>
	constexpr static decltype(auto) unerase(ObjectArg&& arg) {
		using BareObjectArgType = std::decay_t<ObjectArg>;

		if constexpr (std::is_const_v<BareObjectArgType> && std::is_lvalue_reference_v<ObjectArg>) {
			return reinterpret_cast<const SelfType&>(arg);
		} else if constexpr (!std::is_const_v<BareObjectArgType> && std::is_lvalue_reference_v<ObjectArg>) {
			return reinterpret_cast<SelfType&>(arg);
		} else {
			static_assert(false, "Unexpected ObjectArg. Should be a const or non-const lvalue reference.");
		}
	}

	template <class DefaultConstructibleLambdaType, class DefaultConstructibleLambdaTypeSignature, class... InvokeArgs>
	struct Uneraser;

	template <class DefaultConstructibleLambdaType, class FunctorReturnType, class FunctorSelfArg, class... FunctorArgs, class... InvokeArgs>
	struct Uneraser<DefaultConstructibleLambdaType, FunctorReturnType (FunctorSelfArg, FunctorArgs...), InvokeArgs...> {
		static MappingReturnType invoke(ObjectArg object, InvokeArgs... invokeArgs) {
			return DefaultConstructibleLambdaType{}(
				unerase<FunctorSelfArg>(std::forward<ObjectArg>(object)),
				std::forward<InvokeArgs>(invokeArgs)...
				);
		}
	};

	FunctionPtr function_;

};

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_METHOD_HPP__ */