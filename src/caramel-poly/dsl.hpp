// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_DSL_HPP__
#define CARAMELPOLY_DSL_HPP__

#include "detail/ConstexprList.hpp"
#include "detail/ConstexprPair.hpp"
#include "detail/ConstexprString.hpp"
#include "SelfPlaceholder.hpp"

namespace caramel::poly {

template <class Signature>
struct Function {
	using Type = Signature;
};

template <class Sig1, class Sig2>
constexpr auto operator==(Function<Sig1>, Function<Sig2>) {
	return std::is_same_v<Sig1, Sig2>;
}

template <class Sig1, class Sig2>
constexpr auto operator!=(Function<Sig1> m1, Function<Sig2> m2) {
	return !(m1 == m2);
}

// Right-hand-side of a clause in a concept that signifies a function with the given signature.
template <class Signature>
constexpr auto function = Function<Signature>{};

template <class Signature>
struct Method;

// Right-hand-side of a clause in a concept that signifies a method with the
// given signature. The first parameter of the resulting function is implicitly
// `caramel::poly::SelfPlaceholder&` for a non-const method, and
// `const caramel::poly::SelfPlaceholder&` for a const method.
template <class Signature>
constexpr auto method = Method<Signature>{};

template <class R, class... Args>
struct Method<R(Args...)> {
	using Type = R (caramel::poly::SelfPlaceholder&, Args...);
};

template <class R, class... Args>
struct Method<R(Args...) &> {
	using Type = R (caramel::poly::SelfPlaceholder&, Args...);
};

template <class R, class... Args>
struct Method<R(Args...) &&> {
	using Type = R (caramel::poly::SelfPlaceholder&&, Args...);
};

template <class R, class... Args>
struct Method<R(Args...) const> {
	using Type = R (const caramel::poly::SelfPlaceholder&, Args...);
};

template <class R, class... Args>
struct Method<R(Args...) const&> {
	using Type = R (const caramel::poly::SelfPlaceholder&, Args...);
};

// const&& not supported because it's stupid

template <class Sig1, class Sig2>
constexpr auto operator==(Method<Sig1>, Method<Sig2>) {
	return std::is_same_v<Sig1, Sig2>;
}

template <class Sig1, class Sig2>
constexpr auto operator!=(Method<Sig1> m1, Method<Sig2> m2) {
	return !(m1 == m2);
}

namespace detail {

template <class Name, class... Args>
class DelayedCall {
public:

	ConstexprList<Args...> args;

	// All the constructors are private so that only `caramel::poly::MethodName` can
	// construct an instance of this. The intent is that we can only
	// manipulate temporaries of this type.

private:

	template <char... CHARS>
	friend struct MethodName;

	template <class... T>
	constexpr DelayedCall(T&&... t) :
		args(std::forward<T>(t)...)
	{
	}

	DelayedCall(const DelayedCall&) = default;
	DelayedCall(DelayedCall&&) = default;

};

template <char... CHARS>
struct MethodName : ConstexprString<CHARS...> {

	template <class Function>
	constexpr ConstexprPair<MethodName, Function> operator=(Function f) const {
		static_assert(
			std::is_empty_v<Function>,
			"Only stateless function objects can be used to define vtables"
			);
		return { {}, f };
	}

	template <class... Args>
	constexpr auto operator()(Args&&... args) const {
		return DelayedCall<MethodName, Args&&...>{ std::forward<Args>(args)... };
	}

};

template <class S, std::size_t... N>
constexpr MethodName<S::get()[N]...> prepareStringImpl(std::index_sequence<N...>) {
	return {};
}

template <class S>
constexpr auto prepareString(S) {
	return prepareStringImpl<S>(std::make_index_sequence<S::size()>{});
}

} // namespace detail

#ifdef CARAMEL_POLY_ENABLE_STRING_USER_LITERAL

inline namespace literals {

// Creates a compile-time string that can be used as the left-hand-side when
// defining clauses or filling concept maps.
template <class CharT, CharT... CHARS>
constexpr auto operator""_s() {
	return detail::MethodName<CHARS...>{};
}

} // namespace literals

#endif /* CARAMEL_POLY_ENABLE_STRING_USER_LITERAL */

// Creates a MethodName without requiring the use of a user-defined literal.
//
// The user-defined literal is non-standard as of C++17, and it requires
// brining the literal in scope (through a using declaration or such),
// which is not always convenient or possible.
#define POLY_FUNCTION_LABEL(s)                                                   \
  (::caramel::poly::detail::prepareString([]{                             \
	  struct tmp {                                                       \
		  /* exclude null terminator in size() */                        \
		  static constexpr std::size_t size() { return sizeof(s) - 1; }  \
		  static constexpr char const* get() { return s; }               \
	  };                                                                 \
	  return tmp{};                                                      \
  }()))                                                                  \
/**/

} // namespace caramel::poly

#endif /* CARAMELPOLY_DSL_HPP__ */
