// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_DETAIL_BINDSIGNATURE_HPP__
#define CARAMELPOLY_DETAIL_BINDSIGNATURE_HPP__

#include "TransformSignature.hpp"
#include "../SelfPlaceholder.hpp"

namespace caramel::poly::detail {

template <class Old, class New, class T>
struct ReplaceImpl {
	using Type = T;
};

template <class Old, class New>
struct ReplaceImpl<Old, New, Old> {
	using Type = New;
};

template <class Old, class New>
struct ReplaceImpl<Old, New, Old&> {
	using Type = New&;
};
template <class Old, class New>
struct ReplaceImpl<Old, New, Old&&> {
	using Type = New&&;
};
template <class Old, class New>
struct ReplaceImpl<Old, New, Old*> {
	using Type = New*;
};

template <class Old, class New>
struct ReplaceImpl<Old, New, Old const> {
	using Type = New const;
};

template <class Old, class New>
struct ReplaceImpl<Old, New, Old const&> {
	using Type = New const&;
};

template <class Old, class New>
struct ReplaceImpl<Old, New, Old const&&> {
	using Type = New const&&;
};

template <class Old, class New>
struct ReplaceImpl<Old, New, Old const*> {
	using Type = New const*;
};

template <class Old, class New>
struct Replace {

	template <class T>
	using Type = ReplaceImpl<Old, New, T>;

};

// Takes a placeholder signature and replaces instances of `SelfPlaceholder` by the given
// type.
//
// Basically, this will turn stuff like `void (SelfPlaceholder const&, int, SelfPlaceholder*)`
// into `void (T const&, int, T*)`, where `T` is the type we are _binding_
// the signature to.
//
// This is used to make sure that the functions provided in a trait map
// have the right signature.
template <class Signature, class T>
using BindSignature = TransformSignature<Signature, Replace<SelfPlaceholder, T>::template Type>;

} // namespace caramel::poly::detail

#endif /* CARAMELPOLY_DETAIL_BINDSIGNATURE_HPP__ */
