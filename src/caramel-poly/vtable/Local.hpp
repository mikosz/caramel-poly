#ifndef CARAMELPOLY_VTABLE_LOCAL_HPP__
#define CARAMELPOLY_VTABLE_LOCAL_HPP__

#include "Concept.hpp"

namespace caramel_poly::vtable {

namespace detail {

template <class Signature>
struct ErasedFunction;

template <class ReturnType, class... Args>
struct ErasedFunction<ReturnType (Args...)> {

};

template <class Signature>
constexpr auto eraseSelf(Signature* function);

template <class ReturnType, class... Args>
constexpr auto eraseSelf(ReturnType (function*)(Args...)) {

}

template <class Concept>
struct Methods;

template <class HeadNameString, class HeadSignature, class... TailEntries>
struct Methods<Concept<HeadNameString, HeadSignature, TailEntries...>> : Methods<Concept<TailEntries...>> {
public:

	template <ConceptMap>
	Methods(ConceptMap conceptMap) :
		Methods<Concept<TailEntries...>>(conceptMap),
		method_(eraseSelf(conceptMap.get(HeadNameString{})))
	{
	}

private:

	using Signature = decltype(
		typename Concept<HeadNameString, HeadSignature, TailEntries...>::template methodSignature<HeadNameString>::MappingSignature
		);

	Signature* method_;

};

} // namespace detail

template <class Concept>
class Local {
public:

	template <class ConceptMap>
	constexpr Local(ConceptMap conceptMap) :
		methods_(conceptMap)
	{
	}

	template <class ReturnType, class... Args>
	constexpr ReturnType invoke(const_string::ConstString name, Args&&... args) const {
		return methods_[name].invoke<ReturnType>(std::forward<Args>(args)...);
	}

private:

	detail::Methods<Concept> methods_;

};

template <class... Methods>
constexpr auto makeStatic(Methods... methods) {
	return Static<Methods...>(methods...);
}

template <uint32_t MethodNameCrc, class Function>
struct MapEntryKey<Method<MethodNameCrc, Function>> {
	constexpr auto operator()() const {
		return MethodNameCrc;
	}
};

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_LOCAL_HPP__ */
