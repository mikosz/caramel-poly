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

	class ErasedFunction {
	public:

		// TODO: this is not finished (and probably not even correct)
		template <class MappingType>
		struct UnerasedType {
			using Type = MappingType;
		};

		template <>
		struct UnerasedType<vtable::Object&> {
			using Type = SelfType&;
		};

		template <>
		struct UnerasedType<const vtable::Object&> {
			using Type = const SelfType&;
		};

		template <class MappingType>
		constexpr static decltype(auto) unerase(MappingType&& arg) {
			if constexpr (std::is_same_v<MappingType, vtable::Object&>) {
				return reinterpret_cast<SelfType&>(arg);
			} else if constexpr (std::is_same_v<MappingType, const vtable::Object&>) {
				return reinterpret_cast<const SelfType&>(arg);
			} else {
				return std::forward<MappingType>(arg);
			}
		}

		using UnerasedFunctionPtr = MappingReturnType (*)(UnerasedType<MappingArgs>...);

		constexpr ErasedFunction(UnerasedFunctionPtr function) :
			function_(std::move(function))
		{
		}

		MappingReturnType invoke(MappingArgs... mappingArgs) const {
			return (*function_)(unerase(std::forward<MappingArgs>(mappingArgs))...);
		}

	private:

		UnerasedFunctionPtr function_;

	};

	constexpr Method(typename ErasedFunction::UnerasedFunctionPtr function) :
		function_(std::move(function))
	{
	}

	template <class RequestReturnType, class... RequestArgs>
	RequestReturnType invoke(RequestArgs&&... requestArgs) const {
		return function_.invoke(std::forward<RequestArgs>(requestArgs)...);
	}

private:

	ErasedFunction function_;

};

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_METHOD__ */
