#ifndef CARAMELPOLY_DETAIL_METHOD__
#define CARAMELPOLY_DETAIL_METHOD__

#include <type_traits>

#include "caramel-poly/vtable/MethodSignature.hpp"

namespace caramel_poly::detail {

template <class SelfType, class MappingSignature>
class Method;

template <class SelfType, class MappingReturnType, class... MappingArgs>
class Method<SelfType, MappingReturnType (MappingArgs...)> {
public:

	using FunctionPtr = MappingReturnType (*)(MappingArgs...);

	struct ErasedFunction {

		constexpr ErasedFunction(FunctionPtr function) :
			function_(std::move(function))
		{
		}

		template <class T, class MappingType>
		constexpr static auto unerase(MappingType&& arg) {
			// TODO: this is not finished (and probably not even correct)
			if constexpr (std::is_same_v<MappingType, vtable::Object&>) {
				return static_cast<SelfType&>(arg);
			} else if constexpr (std::is_same_v<MappingType, const vtable::Object&>) {
				return static_cast<const SelfType&>(arg);
			} else {
				return std::forward<MappingType>(arg);
			}
		}

		static MappingReturnType invoke(MappingArgs... mappingArgs) {
			return (*function_)(unerase(std::forward<MappingArgs>(mappingArgs)...));
		}

		FunctionPtr function_;

	};

	constexpr Method(FunctionPtr function) :
		function_(std::move(function))
	{
	}

	template <class RequestReturnType, class... RequestArgs>
	RequestReturnType invoke(RequestArgs&&... requestArgs) const {
		return (*function_)(std::forward<RequestArgs>(requestArgs)...);
	}

private:

	ErasedFunction function_;

};

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_METHOD__ */
