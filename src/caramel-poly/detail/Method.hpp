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

	template <class Functor>
	constexpr Method([[maybe_unused]] Functor functor) :
		function_(Uneraser<Functor>::invoke)
	{
	}

	template <class SelfType, class ReturnType, class... Args>
	ReturnType invoke(SelfType&& self, Args&&... Args) const {
		return function_.invoke(erase(std::forward<SelfType>(self)), std::forward<RequestArgs>(requestArgs)...);
	}

private:

	template <class SelfType>
	constexpr static decltype(auto) erase(SelfType&& arg) {
		if constexpr (std::is_const_v<SelfType> && std::is_lvalue_reference_v<SelfType>) {
			return reinterpret_cast<const Object&>(arg);
		} else if constexpr (!std::is_const_v<SelfType> && std::is_lvalue_reference_v<SelfType>) {
			return reinterpret_cast<Object&>(arg);
		} else {
			static_assert(false, "incomplete...");
		}
	}

	template <class Functor>
	struct Uneraser {
		static MappingReturnType invoke(MappingArgs&&... mappingArgs) {
			Functor{}(std::forward);
		}
	};

	FunctionPtr function_;

};

} // namespace caramel_poly::detail

#endif /* CARAMELPOLY_DETAIL_METHOD__ */



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
