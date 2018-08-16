// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_DSL_HPP__
#define CARAMELPOLY_DSL_HPP__

#include "SelfPlaceholder.hpp"

namespace caramel_poly {

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
// `caramel_poly::SelfPlaceholder&` for a non-const method, and
// `const caramel_poly::SelfPlaceholder&` for a const method.
template <class Signature>
constexpr auto method = Method<Signature>{};

template <class R, class... Args>
struct Method<R(Args...)> {
	using Type = R (caramel_poly::SelfPlaceholder&, Args...);
};

template <class R, class... Args>
struct Method<R(Args...) &> {
	using Type = R (caramel_poly::SelfPlaceholder&, Args...);
};

template <class R, class... Args>
struct Method<R(Args...) &&> {
	using Type = R (caramel_poly::SelfPlaceholder&&, Args...);
};

template <class R, class... Args>
struct Method<R(Args...) const> {
	using Type = R (const caramel_poly::SelfPlaceholder&, Args...);
};

template <class R, class... Args>
struct Method<R(Args...) const&> {
	using Type = R (const caramel_poly::SelfPlaceholder&, Args...);
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
struct delayed_call {
	boost::hana::tuple<Args...> args;

	// All the constructors are private so that only `dyno::string` can
	// construct an instance of this. The intent is that we can only
	// manipulate temporaries of this type.
private:
	template <char... c> friend struct string;

	template <class... T>
	constexpr delayed_call(T&&... t) : args{std::forward<T>(t)...} { }
	delayed_call(delayed_call const&) = default;
	delayed_call(delayed_call&&) = default;
};

template <char... c>
struct string : boost::hana::string<c...> {
	template <class Function>
	constexpr boost::hana::pair<string, Function>
	operator=(Function f) const {
		static_assert(std::is_empty<Function>{},
			"Only stateless function objects can be used to define vtables");
		return {{}, f};
	}

	template <class... Args>
	constexpr auto operator()(Args&&... args) const {
		return detail::delayed_call<string, Args&&...>{std::forward<Args>(args)...};
	}

	using hana_tag = class boost::hana::tag_of<boost::hana::string<c...>>::type;
};

template <class S, std::size_t... N>
constexpr detail::string<S::get()[N]...> prepare_string_impl(std::index_sequence<N...>)
{ return {}; }

template <class S>
constexpr auto prepare_string(S) {
	return detail::prepare_string_impl<S>(std::make_index_sequence<S::size()>{});
}

} // namespace detail

inline namespace literals {

// Creates a compile-time string that can be used as the left-hand-side when
// defining clauses or filling concept maps.
template <class CharT, CharT... c>
constexpr auto operator""_s() { return detail::string<c...>{}; }

} // namespace literals

// Creates a Dyno compile-time string without requiring the use of a
// user-defined literal.
//
// The user-defined literal is non-standard as of C++17, and it requires
// brining the literal in scope (through a using declaration or such),
// which is not always convenient or possible.
#define DYNO_STRING(s)													  \
  (::dyno::detail::prepare_string([]{									   \
	  struct tmp {														  \
		  /* exclude null terminator in size() */						   \
		  static constexpr std::size_t size() { return sizeof(s) - 1; }	 \
		  static constexpr char const* get() { return s; }				  \
	  };																	\
	  return tmp{};														 \
  }()))																	 \
/**/

} // namespace caramel_poly

#endif /* CARAMELPOLY_DSL_HPP__ */
