// Copyright Mikolaj Radwan 2018
// Distributed under the MIT license (See accompanying file LICENSE)
//
// This is Louis Dionne's Dyno library adapted to work on Visual Studio 2017 and
// without Boost, modified to my taste. All credit for the design and delivery goes
// to Louis. His original implementation may be found here:
// https://github.com/ldionne/dyno

#ifndef CARAMELPOLY_CONSTEXPRSTRING_HPP__
#define CARAMELPOLY_CONSTEXPRSTRING_HPP__

#include <utility>
#include <iosfwd>

namespace caramel_poly {

template <char... CHARS>
class ConstexprString final {
public:

	constexpr ConstexprString() = default;

	constexpr size_t length() const {
		return sizeof...(CHARS);
	}

	constexpr const char* c_str() const {
		return string_detail::COMPILE_STRING_STORAGE<CHARS...>;
	}

	constexpr char operator[](size_t index) const {
		return c_str()[index];
	}

	template <char... OTHER_CHARS>
	constexpr bool operator==([[maybe_unused]] ConstexprString<OTHER_CHARS...> other) const {
		return std::is_same_v<ConstexprString, ConstexprString<OTHER_CHARS...>>;
	}

	template <char... OTHER_CHARS>
	constexpr bool operator!=(ConstexprString<OTHER_CHARS...> other) const {
		return !(*this == other);
	}

};

template <char... CHARS>
std::ostream& operator<<(std::ostream& os, ConstexprString<CHARS...> s) {
	return os << s.c_str();
}

namespace string_detail {

template <char... CHARS>
constexpr const char COMPILE_STRING_STORAGE[] = { CHARS..., '\0' };

template <class S, std::size_t... N>
constexpr auto prepare_impl(S, std::index_sequence<N...>)
{
	return ConstexprString<S::get()[N]...>{};
}

template <typename S>
constexpr decltype(auto) prepare(S s) {
	return prepare_impl(s, std::make_index_sequence<sizeof(S::get()) - 1>{});
}

} // namespace string_detail

} // namespace caramel_poly

#define CONSTEXPR_STRING(s)                                                 \
    (::caramel_poly::string_detail::prepare([]{                             \
        struct tmp {                                                        \
            static constexpr decltype(auto) get() {                         \
				return s;                                                   \
			}                                                               \
        };                                                                  \
        return tmp{};                                                       \
    }()))

#endif /* CARAMELPOLY_CONSTEXPRSTRING_HPP__ */
