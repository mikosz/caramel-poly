#ifndef CARAMELPOLY_DETAIL_METHOD__
#define CARAMELPOLY_DETAIL_METHOD__

#include <type_traits>

#include "caramel-poly/vtable/Object.hpp"
#include "caramel-poly/vtable/MethodSignature.hpp"

namespace caramel_poly::detail {

template <class MappingSignature>
class Method;

template <class MappingReturnType, class... MappingArgs>
class Method<MappingReturnType (MappingArgs...)> {
public:

	using FunctionPtr = MappingReturnType (*)(MappingArgs...);

	template <class DefaultConstructibleLambdaType>
	constexpr Method([[maybe_unused]] DefaultConstructibleLambdaType default_constructible_lambda) :
		function_(&Uneraser<DefaultConstructibleLambdaType, typename DefaultConstructibleLambdaType::Signature, MappingArgs...>::invoke)
	{
	}

	template <class SelfType, class... Args>
	decltype(auto) invoke(SelfType&& self, Args... args) const {
		return (*function_)(erase(std::forward<SelfType>(self)), std::forward<Args>(args)...);
	}

private:

	template <class SelfType>
	constexpr static decltype(auto) erase(SelfType&& arg) {
		return reinterpret_cast<const Object&>(arg);
		//if constexpr (std::is_const_v<SelfType> && std::is_lvalue_reference_v<SelfType>) {
		//	return reinterpret_cast<const Object&>(arg);
		//} else if constexpr (!std::is_const_v<SelfType> && std::is_lvalue_reference_v<SelfType>) {
		//	return reinterpret_cast<Object&>(arg);
		//} else if constexpr (std::is_const_v<SelfType> && std::is_rvalue_reference_v<SelfType>) {
		//	return reinterpret_cast<const Object&&>(arg);
		//} else if constexpr (!std::is_const_v<SelfType> && std::is_rvalue_reference_v<SelfType>) {
		//	return reinterpret_cast<Object&&>(arg);
		//} else {
		//	static_assert(false, "incomplete...");
		//}
	}

	template <class SelfType, class ObjectArg>
	constexpr static decltype(auto) unerase(ObjectArg arg) {
		if constexpr (std::is_const_v<ObjectType> && std::is_lvalue_reference_v<ObjectArg>) {
			return reinterpret_cast<const SelfType&>(arg);
		} else if constexpr (!std::is_const_v<ObjectArg> && std::is_lvalue_reference_v<ObjectArg>) {
			return reinterpret_cast<SelfType&>(arg);
		} else {
			static_assert(false, "incomplete...");
		}
	}

	template <class DefaultConstructibleLambdaType, class DefaultConstructibleLambdaTypeSignature, class... InvokeArgs>
	struct Uneraser;

	template <class DefaultConstructibleLambdaType, class FunctorReturnType, class FunctorSelfArg, class... FunctorArgs, class ObjectArg, class... InvokeArgs>
	struct Uneraser<DefaultConstructibleLambdaType, FunctorReturnType (FunctorSelfArg, FunctorArgs...), ObjectArg, InvokeArgs...> {
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

#endif /* CARAMELPOLY_DETAIL_METHOD__ */
