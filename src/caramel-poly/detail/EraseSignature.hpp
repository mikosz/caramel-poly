// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_DETAIL_ERASESIGNATURE_HPP__
#define CARAMELPOLY_DETAIL_ERASESIGNATURE_HPP__

#include "EraserTraits.hpp"
#include "TransformSignature.hpp"

namespace caramel_poly::detail {

template <typename Eraser>
struct ApplyErasePlaceholder {

	template <typename Placeholder>
	using Type = ErasePlaceholder<Eraser, Placeholder>;

};

// Transforms a signature potentially containing placeholders into a signature
// containing no placeholders, and which would be suitable for storing as a
// function pointer.
//
// Basically, this turns types like `caramel_poly::SelfPlaceholder&` into `void*`
// (or what's specified by the given `Eraser`) at the top-level of the signature.
// This is used when we need to generate a vtable from a concept definition. The
// concept defines signatures with placeholders, and we need to generate a concrete
// function type that can be stored in a vtable. That concrete type is the result of
// `EraseSignature`.
//
// Note that this returns a function type, not a function pointer type.
// For actually storing an object of this type, one needs to add a pointer
// qualifier to it.

template <typename Signature, typename Eraser = void>
using EraseSignature = TransformSignature<Signature, ApplyErasePlaceholder<Eraser>::template Type>;

} // namespace caramel_poly::detail

#endif // CARAMELPOLY_DETAIL_ERASESIGNATURE_HPP__
