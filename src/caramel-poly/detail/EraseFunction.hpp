// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_DETAIL_ERASEFUNCTION_HPP__
#define CARAMELPOLY_DETAIL_ERASEFUNCTION_HPP__

#include <utility>

#include "boost_callable_traits/function_type.hpp"
#include "EmptyObject.hpp"
#include "EraserTraits.hpp"

namespace caramel_poly::detail {

template <class Eraser, class F, class PlaceholderSig, class ActualSig>
struct Thunk;

template <class Eraser, class F, class R_pl, class... Args_pl, class R_ac, class... Args_ac>
struct Thunk<Eraser, F, R_pl (Args_pl...), R_ac (Args_ac...)> {
	
	static constexpr auto apply(typename ErasePlaceholder<Eraser, Args_pl>::Type... args)
		-> typename ErasePlaceholder<Eraser, R_pl>::Type
	{
		return Erase<Eraser, R_pl>::apply(
			EmptyObject<F>::get()(
				Unerase<Eraser, Args_pl, Args_ac>::apply(
					std::forward<typename ErasePlaceholder<Eraser, Args_pl>::Type>(args)
					)...
				)
			);
	}

};

template <class Eraser, class F, /* void, */ class... Args_pl, class R_ac, class... Args_ac>
struct Thunk<Eraser, F, void (Args_pl...), R_ac (Args_ac...)> {

	static constexpr auto apply(typename ErasePlaceholder<Eraser, Args_pl>::Type... args)
		-> void
	{
		EmptyObject<F>::get()(
			Unerase<Eraser, Args_pl, Args_ac>::apply(
				std::forward<typename ErasePlaceholder<Eraser, Args_pl>::Type>(args)
				)...
			);
	}

};

// Transform an actual (stateless) function object with statically typed
// parameters into a type-erased function suitable for storage in a vtable.
//
// The pointer returned by `EraseFunction` is what's called a Thunk; it
// makes a few adjustments to the arguments (usually 0-overhead static
// casts) and forwards them to another function.
//
// TODO:
//  - Would it be possible to erase a callable that's not a stateless function
//    object? Would that necessarily require additional storage?
//  - Should we be returning a lambda that erases its arguments?
template <class Signature, class Eraser = void, class F>
constexpr auto EraseFunction(const F&) {
	using ActualSignature = boost::callable_traits::function_type_t<F>;
	using Thunk = Thunk<Eraser, F, Signature, ActualSignature>;
	return &Thunk::apply;
}

} // namespace caramel_poly::detail

#endif // CARAMELPOLY_DETAIL_ERASEFUNCTION_HPP__
