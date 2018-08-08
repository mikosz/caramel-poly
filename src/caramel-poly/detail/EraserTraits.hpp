// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_DETAIL_ERASERTRAITS_HPP__
#define CARAMELPOLY_DETAIL_ERASERTRAITS_HPP__

#include <type_traits>
#include <utility>

namespace caramel_poly {

struct SelfPlaceholder;

} // namespace caramel_poly

namespace caramel_poly::detail {

// Traits for types that can be used to erase other types. The following traits
// should be provided:
//
// template <class Eraser, class Placeholder>
// struct ErasePlaceholder;
// 
// Metafunction transforming the type of a possibly cv and ref-qualified
// placeholder (a `caramel_poly::SelfPlaceholder`) into a representation suitable
// for passing around as a parameter to an erased function. Basically, this turns a
// type like `caramel_poly::SelfPlaceholder&` into a `void*`, and similarly for other
// types of possibly, const or ref-qualified `caramel_poly::SelfPlaceholder`s, but it
// can be customized for custom eraser types.
// 
// template <class Eraser, class Placeholder, class Actual>
// static constexpr ErasePlaceholder<Eraser, Placeholder>::Type
//     erase<Eraser, Placeholder>::apply(Actual);
//
// Function transforming an object to the generic representation for the
// given placeholder for that eraser. This is used to obtain a representation
// of the object that can be passed to an erased function stored in a vtable.
// For example, in the case of the `void` eraser, this function just takes a
// reference or a pointer to an actual type and passes it as a `void*`, with
// the proper cv qualifiers. Note that an eraser is not expected to support
// erasure of arbitrary types. For example, it is perfectly fine to give an
// error if one tries to erase an `int` as a `caramel_poly::SelfPlaceholder&`,
// since that makes no sense. However, it is probably a good idea to support
// inexact casts that do make sense, such as erasing `int&` to
// `caramel_poly::SelfPlaceholder const&` (even though the cv-qualifiers don't
// match). This is left to the implementation of the specific eraser.
//
// template <class Eraser, class Placeholder, class Actual>
// static constexpr Actual
//     unerase<Eraser, Placeholder, Actual>::apply(ErasePlaceholder<Eraser, Placeholder>::Type)
//
// This is the inverse operation to `erase`. It takes an object that was
// erased and interprets it as an object of the specified `Actual` type.
// The eraser can assume that the object that was erased is indeed of the
// requested type. This function is used to transform an object with an
// erased representation into an object that can be passed to a function
// stored in a concept map.

template <class Eraser, class T>
struct ErasePlaceholder {

	static_assert(!std::is_same<T, caramel_poly::SelfPlaceholder>{},
		"caramel_poly::SelfPlaceholder may not be passed by value; it is only a placeholder");

	using Type = T;

};

template <class Eraser, class T>
struct Erase {

	template <class Arg>
	static constexpr decltype(auto) apply(Arg&& arg) {
		return std::forward<Arg>(arg);
	}

};

template <class Eraser, class T, class Actual>
struct Unerase {

	template <class Arg>
	static constexpr decltype(auto) apply(Arg&& arg) {
		return std::forward<Arg>(arg);
	}

};

// Specialization for the `void` Eraser, which uses `void*` to erase types.
template <>
struct ErasePlaceholder<void, const caramel_poly::SelfPlaceholder&> {
	using Type = const void*;
};

template <>
struct ErasePlaceholder<void, caramel_poly::SelfPlaceholder&> {
	using Type = void*;
};

template <>
struct ErasePlaceholder<void, caramel_poly::SelfPlaceholder&&> {
	using Type = void*;
};

template <>
struct ErasePlaceholder<void, caramel_poly::SelfPlaceholder*> {
	using Type = void*;
};

template <>
struct ErasePlaceholder<void, const caramel_poly::SelfPlaceholder*> {
	using Type = const void*;
};

template <>
struct Erase<void, const caramel_poly::SelfPlaceholder&> {

	template <class Arg>
	static constexpr const void* apply(const Arg& arg) { 
		return &arg;
	}

};

template <>
struct Erase<void, caramel_poly::SelfPlaceholder&> {

	template <class Arg>
	static constexpr void* apply(Arg& arg) {
		return &arg;
	}

};

template <>
struct Erase<void, caramel_poly::SelfPlaceholder&&> {

	template <class Arg>
	static constexpr void* apply(Arg&& arg) {
		static_assert(std::is_rvalue_reference_v<decltype(std::forward<Arg>(arg))>, "would move from non-rvalue");
		return &arg;
	}

};

template <>
struct Erase<void, const caramel_poly::SelfPlaceholder*> {

	template <class Arg>
	static constexpr const void* apply(const Arg* arg) {
		return arg;
	}

};
template <>
struct Erase<void, caramel_poly::SelfPlaceholder*> {

	template <class Arg>
	static constexpr void* apply(Arg* arg) {
		return arg;
	}

};

template <class Actual>
struct Unerase<void, caramel_poly::SelfPlaceholder&, Actual&> {

	static constexpr Actual& apply(void* arg) {
		return *static_cast<Actual*>(arg);
	}

};

template <class Actual>
struct Unerase<void, const caramel_poly::SelfPlaceholder&, const Actual&> {

	static constexpr const Actual& apply(const void* arg) {
		return *static_cast<const Actual*>(arg);
	}

};

template <class Actual>
struct Unerase<void, caramel_poly::SelfPlaceholder&&, Actual&&> {

	static constexpr Actual&& apply(void* arg) {
		return std::move(*static_cast<Actual*>(arg));
	}

};

template <class Actual>
struct Unerase<void, caramel_poly::SelfPlaceholder*, Actual*> {

	static constexpr Actual* apply(void* arg) {
		return static_cast<Actual*>(arg);
	}

};

template <class Actual>
struct Unerase<void, const caramel_poly::SelfPlaceholder*, const Actual*> {

	static constexpr const Actual* apply(const void* arg) {
		return static_cast<const Actual*>(arg);
	}

};

} // namespace caramel_poly::detail

#endif // CARAMELPOLY_DETAIL_ERASERTRAITS_HPP__
