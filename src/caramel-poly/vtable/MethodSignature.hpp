#ifndef CARAMELPOLY_VTABLE_METHODSIGNATURE_HPP__
#define CARAMELPOLY_VTABLE_METHODSIGNATURE_HPP__

#include "Object.hpp"

namespace caramel_poly::vtable {

template <class Signature>
struct MethodSignature;

template <class ReturnType, class... Args>
struct MethodSignature<ReturnType (Args...)> {
	using MappingSignature = ReturnType (Object&, Args...);
};

template <class ReturnType, class... Args>
struct MethodSignature<ReturnType (&)(Args...)> {
	using MappingSignature = ReturnType (Object&, Args...);
};

template <class ReturnType, class... Args>
struct MethodSignature<ReturnType (&&)(Args...)> {
	using MappingSignature = ReturnType (Object&&, Args...);
};

template <class ReturnType, class... Args>
struct MethodSignature<ReturnType (Args...) const> {
	using MappingSignature = ReturnType (const Object&, Args...);
};

template <class ReturnType, class... Args>
struct MethodSignature<ReturnType (&)(Args...) const> {
	using MappingSignature = ReturnType (const Object&, Args...);
};

template <class ReturnType, class... Args>
struct MethodSignature<ReturnType (&&)(Args...) const> {
	using MappingSignature = ReturnType (const Object&&, Args...);
};

} // namespace caramel_poly::vtable

#endif /* CARAMELPOLY_VTABLE_METHODSIGNATURE_HPP__ */
